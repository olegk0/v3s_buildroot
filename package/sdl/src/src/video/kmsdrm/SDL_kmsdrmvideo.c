// SPDX-License-Identifier: LGPL-2.1-or-later
/*
 * Copyright (C) 2020 João H. Spies <johnnyonflame@hotmail.com>
 *
 * KMS/DRM video backend code for SDL1
 */

#define _FILE_OFFSET_BITS 64
#include "SDL_config.h"

#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "SDL_endian.h"
#include "SDL_timer.h"
#include "SDL_thread.h"
#include "SDL_video.h"
#include "SDL_mouse.h"
#include "../SDL_sysvideo.h"
#include "../SDL_pixels_c.h"
#include "../../events/SDL_events_c.h"

#include "SDL_kmsdrmvideo.h"
#include "SDL_kmsdrmevents_c.h"
#include "SDL_kmsdrmmisc_c.h"
#include "SDL_kmsdrmcolordef.h"

#define KMSDRM_DRIVER_NAME "kmsdrm"

static int
KMSDRM_TripleBufferingThread(void* d);
static void KMSDRM_TripleBufferInit(_THIS);
static void KMSDRM_TripleBufferStop(_THIS);
static void KMSDRM_TripleBufferQuit(_THIS);

static void
KMSDRM_DeleteDevice(SDL_VideoDevice* device)
{
  // printf("KMSDRM_DeleteDevice\n");
  SDL_free(device->hidden);
  SDL_free(device);
}

static int
KMSDRM_Available(void)
{
  // printf("KMSDRM_Available\n");
  const char* envr = SDL_getenv("SDL_VIDEODRIVER");
  if ((envr) && (SDL_strcmp(envr, KMSDRM_DRIVER_NAME) == 0)) {
    return (1);
  }

  DhHwInfo* dhHwInfo = DrmHelperInit(0);

  if (!dhHwInfo) {
    printf("Error DrmHelperInit\n");
    return 0;
  }

  DrmHelperFree();
  return (1);
}

int
KMSDRM_VideoInit(_THIS, SDL_PixelFormat* vformat)
{
  // printf("KMSDRM_VideoInit\n");
  drm_hw_info = DrmHelperInit(0);

  if (!drm_hw_info) {
    SDL_SetError("Could not find any (capable) DRM device.\n");
    return -1;
  }

  // Initialize vid_mode listing
  drm_vid_mode_count = 0;
  drm_vid_modes = SDL_realloc(
    drm_vid_modes, sizeof(*drm_vid_modes) * (drm_vid_mode_count + 1));
  drm_vid_modes[0] = NULL;

  drm_crtc_w = drm_hw_info->width;
  drm_crtc_h = drm_hw_info->height;
  KMSDRM_RegisterVidMode(this, drm_hw_info->width, drm_hw_info->height);

  // Setup video information
  this->info.hw_available = 1;
  if (drm_vid_modes[0]) {
    this->info.current_w = drm_vid_modes[0]->w;
    this->info.current_h = drm_vid_modes[0]->h;
    vformat->BitsPerPixel = drm_hw_info->bpp;
  }

  drm_plane_info = NULL;
  KMSDRM_TripleBufferInit(this);

  if (KMSDRM_InitInput(this)) {
    SDL_SetError("Unable KMSDRM_InitInput");
    DrmHelperFree();
    return -1;
  }

  if (FB_EnterGraphicsMode(this) < 0) {
    SDL_SetError("Unable FB_EnterGraphicsMode");
    KMSDRM_ExitInput(this);
    DrmHelperFree();
    return -1;
  }

  return 0;
}

SDL_Rect**
KMSDRM_ListModes(_THIS, SDL_PixelFormat* format, Uint32 flags)
{
  // printf("KMSDRM_ListModes\n");
  return drm_vid_modes;
}

static int
KMSDRM_VideoModeOK(_THIS, int width, int height, int bpp, Uint32 flags)
{
  // printf("KMSDRM_VideoModeOK  width:%d height:%d\n",width, height);
  if (width > drm_hw_info->width || height > drm_hw_info->height)
    return 0;
  return (bpp); /* TODO: check that the resolution is really OK */
}

static void
KMSDRM_ClearShadowbuffer(_THIS)
{
  if (drm_shadow_buffer) {
    free(drm_shadow_buffer);
    // free(drm_yuv_palette);
    drm_shadow_buffer = NULL;
    // drm_yuv_palette = NULL;
  }
}

void
KMSDRM_ShowLayer(_THIS, char show)
{
  if (drm_plane_info) {
    if (show)
      DrmHelperSetZpos(drm_plane_info, 1);
    else
      DrmHelperSetZpos(drm_plane_info, 0);
  }
}

static SDL_Surface*
KMSDRM_SetVideoMode(_THIS,
                    SDL_Surface* current,
                    int width,
                    int height,
                    int bpp,
                    Uint32 flags)
{
  // printf("KMSDRM_SetVideoMode  width:%d height:%d\n",width, height);
  //  Lock the event thread, in multi-threading environments
  SDL_Lock_EventThread();

  // If we have set a video mode previously, now we need to clean up.
  if (drm_plane_info) {
    if (drm_triplebuf_thread) {
      KMSDRM_TripleBufferStop(this);
    }

    DrmHelperFreeFb(drm_plane_info);
    drm_plane_info = NULL;
  }

  KMSDRM_ClearShadowbuffer(this);

  const drm_color_def* color_def;
  if (bpp == 8) {
    /**
     * Emulate paletted video mode. For this,
     * it's necessary to create a shadow one here instead of
     * letting SDL handle it.
     **/
    drm_shadow_buffer = calloc(width * height + 10, 1);
    //  drm_yuv_palette = calloc(1 << bpp, sizeof(*drm_yuv_palette));
    if (!drm_shadow_buffer) {
      SDL_SetError("Error alloc shadow buffer.\n");
      goto setvidmode_fail;
    }
    // emulate  use 32 bit overlay
    color_def = get_drm_color_def(32, flags);
  } else {
    color_def = get_drm_color_def(bpp, flags);
  }

  if (!color_def) {
    SDL_SetError("Bad pixel format (%dbpp).\n", bpp);
    goto setvidmode_fail;
  }

  // Determine how many buffers do we need
  int n_buf = ((flags & SDL_TRIPLEBUF) == SDL_TRIPLEBUF)   ? 3
              : ((flags & SDL_TRIPLEBUF) == SDL_DOUBLEBUF) ? 2
                                                           : 1;
  /*
  if (drm_shadow_buffer) {
    n_buf = 1;
  }
  */
  // Initialize how many framebuffers were requested
  kmsdrm_dbg_printf("Creating %d framebuffers!\n", n_buf);
  // DrmHelperAllocFb(DhPlaneType type, uint32_t fourcc_format, uint16_t width,
  // uint16_t height,uint16_t x, uint16_t y, uint8_t count_bufs, uint8_t
  // fullscreen)
  drm_plane_info = DrmHelperAllocFb(
    dh_plane_type_overlay, color_def->four_cc, width, height, 0, 0, n_buf, 1);
  if (!drm_plane_info) {
    SDL_SetError("Error DrmHelperAllocFb.\n");
    kmsdrm_dbg_printf("Error DrmHelperAllocFb.\n");
    goto setvidmode_fail_fbs;
  }

  // for overlay
  KMSDRM_ShowLayer(this, 1);
  if (n_buf == 1) {
    DrmHelperFlipConfirm(drm_plane_info);
  }

  this->hidden->w = width;
  this->hidden->h = height;
  this->hidden->bpp = bpp;

  // Let SDL know about the created framebuffer
  if (bpp == 8) {
    if (!SDL_ReallocFormat(current, bpp, 0, 0, 0, 0)) {
      SDL_SetError("Unable to recreate surface format structure!\n");
      goto setvidmode_fail_req;
    }
  } else {
    if (!SDL_ReallocFormat(current,
                           bpp,
                           color_def->r_mask,
                           color_def->g_mask,
                           color_def->b_mask,
                           color_def->a_mask)) {
      SDL_SetError("Unable to recreate surface format structure!\n");
      goto setvidmode_fail_req;
    }
  }

  if (drm_shadow_buffer) {
    current->pixels = drm_shadow_buffer;
    current->pitch = width;
  } else {
    // if ( (flags & SDL_TRIPLEBUF) == SDL_TRIPLEBUF ) {
    current->pixels =
      DrmHelperWorkBuf(drm_plane_info, DrmHelperWorkGetBufId(drm_plane_info))
        .map_planes[0];
    current->pitch = DrmHelperWorkBuf(drm_plane_info, 0).fbi.pitches[0];
  }

  current->w = width;
  current->h = height;

  // this->hidden->has_damage_clips = find_property(this,
  // drm_active_pipe->plane,"FB_DAMAGE_CLIPS");

  // Let SDL know what type of surface this is. In case the user asks for a
  // SDL_SWSURFACE video mode, SDL will silently create a shadow buffer
  // as an intermediary.
  current->flags =
    SDL_HWSURFACE | SDL_FULLSCREEN | SDL_NOFRAME | SDL_PREALLOC |
    (flags & SDL_TRIPLEBUF); /* SDL_TRIPLEBUF implies SDL_DOUBLEBUF */

  /* We do always want the hardware palette if 8bpp is selected */
  if (bpp == 8)
    current->flags |= SDL_HWPALETTE;

  if ((flags & SDL_TRIPLEBUF) == SDL_TRIPLEBUF) {
    SDL_LockMutex(drm_triplebuf_mutex);
    drm_triplebuf_thread_stop = 0;
    drm_triplebuf_thread = SDL_CreateThread(KMSDRM_TripleBufferingThread, this);

    /* Wait until the triplebuf thread is ready */
    SDL_CondWait(drm_triplebuf_cond, drm_triplebuf_mutex);
    SDL_UnlockMutex(drm_triplebuf_mutex);
  }

  // Unlock the event thread, in multi-threading environments
  SDL_Unlock_EventThread();
  return current;

setvidmode_fail_req:
  DrmHelperFreeFb(drm_plane_info);
  drm_plane_info = NULL;
setvidmode_fail_fbs:

setvidmode_fail:
  /* Else we can't do much */
  SDL_Unlock_EventThread();

  return NULL;
}

/* We don't actually allow hardware surfaces other than the main one */
static int
KMSDRM_AllocHWSurface(_THIS, SDL_Surface* surface)
{
  // printf("KMSDRM_AllocHWSurface\n");
  return (-1);
}
static void
KMSDRM_FreeHWSurface(_THIS, SDL_Surface* surface)
{
  // printf("KMSDRM_FreeHWSurface\n");
  return;
}

/* We need to wait for vertical retrace on page flipped displays */
static int
KMSDRM_LockHWSurface(_THIS, SDL_Surface* surface)
{
  // printf("KMSDRM_LockHWSurface\n");
  return (0);
}

static void
KMSDRM_UnlockHWSurface(_THIS, SDL_Surface* surface)
{
  // printf("KMSDRM_UnlockHWSurface\n");
  return;
}

static int
KMSDRM_TripleBufferingThread(void* d)
{
  SDL_VideoDevice* this = d;

  SDL_LockMutex(drm_triplebuf_mutex);
  SDL_CondSignal(drm_triplebuf_cond);

  for (;;) {

    SDL_CondWait(drm_triplebuf_cond, drm_triplebuf_mutex);
    if (drm_triplebuf_thread_stop)
      break;

    // int DrmHelperFlip(DhPlaneInfo *plane_info, uint8_t sync);
    int rc = DrmHelperFlip(drm_plane_info);
    if (rc)
      fprintf(stderr, "Unable to flip buffers: %s\n", strerror(errno));
  }

  SDL_UnlockMutex(drm_triplebuf_mutex);
  return 0;
}

static void
KMSDRM_TripleBufferInit(_THIS)
{
  drm_triplebuf_mutex = SDL_CreateMutex();
  drm_triplebuf_cond = SDL_CreateCond();
  drm_triplebuf_thread = NULL;
}

static void
KMSDRM_TripleBufferStop(_THIS)
{
  SDL_LockMutex(drm_triplebuf_mutex);
  drm_triplebuf_thread_stop = 1;
  SDL_CondSignal(drm_triplebuf_cond);
  SDL_UnlockMutex(drm_triplebuf_mutex);

  SDL_WaitThread(drm_triplebuf_thread, NULL);
  drm_triplebuf_thread = NULL;
}

static void
KMSDRM_TripleBufferQuit(_THIS)
{
  if (drm_triplebuf_thread)
    KMSDRM_TripleBufferStop(this);
  SDL_DestroyMutex(drm_triplebuf_mutex);
  SDL_DestroyCond(drm_triplebuf_cond);
}

static void
KMSDRM_BlitSWBuffer(_THIS, int buf_id)
{
  Uint8* src = drm_shadow_buffer;
  Uint8* dst = (Uint8*)DrmHelperWorkBuf(drm_plane_info, buf_id).map_planes[0];
  unsigned dpitch = DrmHelperWorkBuf(drm_plane_info, 0).fbi.pitches[0];
  // for (int i = 0; i < drm_shadow_buffer_size; i++) {
  for (int h = 0; h < DrmHelperWorkBuf(drm_plane_info, 0).fbi.height; h++) {
    Uint32* ldst = (Uint32*)dst;
    for (int w = 0; w < DrmHelperWorkBuf(drm_plane_info, 0).fbi.width; w++) {
      ldst[w] = drm_palette[src[0]];
      src++;
    }
    dst += dpitch;
  }
}

static int
KMSDRM_FlipHWSurface(_THIS, SDL_Surface* surface)
{
  // printf("KMSDRM_FlipHWSurface\n");
  // printf("flags:%d  drm_plane_info:%p\n", surface->flags, drm_plane_info);
  if (!drm_plane_info)
    return -3;

  // Flip the shadow buffer if present.
  if (drm_shadow_buffer) {
    int cur_buf_id = DrmHelperWorkGetBufId(drm_plane_info);
    if (cur_buf_id < 0)
      return -2;
    KMSDRM_BlitSWBuffer(this, cur_buf_id); //&drm_buffers[drm_back_buffer]);

    if ((surface->flags & SDL_TRIPLEBUF) == SDL_TRIPLEBUF) {
      SDL_LockMutex(drm_triplebuf_mutex);
      SDL_CondSignal(drm_triplebuf_cond);
      SDL_UnlockMutex(drm_triplebuf_mutex);
    } else {
      DrmHelperFlipConfirm(drm_plane_info);
    }
  } else {

    // Either wait for VSync or for buffer acquire
    if ((surface->flags & SDL_TRIPLEBUF) == SDL_TRIPLEBUF) {
      SDL_LockMutex(drm_triplebuf_mutex);
      int cur_buf_id = DrmHelperWorkGetBufId(drm_plane_info);
      if (cur_buf_id >= 0) {
        surface->pixels =
          DrmHelperWorkBuf(drm_plane_info, cur_buf_id).map_planes[0];
      }
      SDL_CondSignal(drm_triplebuf_cond);
      SDL_UnlockMutex(drm_triplebuf_mutex);
    } else {
      DrmHelperFlipConfirm(drm_plane_info);
      surface->pixels =
        DrmHelperWorkBuf(drm_plane_info, drm_plane_info->opened_buf)
          .map_planes[0];
    }
  }
  return 1;
}

static void
KMSDRM_UpdateRects(_THIS, int numrects, SDL_Rect* rects)
{
  if (drm_shadow_buffer) {
    int cur_buf_id = DrmHelperWorkGetBufId(drm_plane_info);
    if (cur_buf_id >= 0) {
      KMSDRM_BlitSWBuffer(this, cur_buf_id);
    }
  }
}

int
KMSDRM_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color* colors)
{
  unsigned int i;
  /*
         |   CODE |BPP| R| G| B| A| RS| GS| BS| AS|
  E_RGBA(RGB565,   16, 5, 6, 5, 0, 11,  5,  0,  0);
  */
  for (i = 0; i < ncolors; i++) {
    if (i + firstcolor < 256) {
      /*
      Uint16 t = (0x1f & (colors[i].b >> 3)) | (0x7e0 & (colors[i].g << 3)) |
                 (0xf800 & (colors[i].r << 8));
      drm_palette[firstcolor + i] = t;
      printf("i:%d c:%X R:%X G:%X B:%X\n",
             i,
             t,
             colors[i].r,
             colors[i].g,
             colors[i].b);
                */
      drm_palette[firstcolor + i] =
        (colors[i].r << 16) | (colors[i].g << 8) | (colors[i].b << 0);
    }
  }
  return (1);
}

/* Note:  If we are terminated, this could be called in the middle of
   another SDL video routine -- notably UpdateRects.
*/
void
KMSDRM_VideoQuit(_THIS)
{
  // printf("KMSDRM_VideoQuit\n");
  if (drm_hw_info) {
    KMSDRM_TripleBufferQuit(this);
    KMSDRM_ClearShadowbuffer(this);
    DrmHelperFree();
    drm_hw_info = NULL;
  }
  KMSDRM_ExitInput(this);
}

static SDL_VideoDevice*
KMSDRM_CreateDevice(int devindex)
{
  // printf("SDL_VideoDevice\n");
  SDL_VideoDevice* device;

  /* Initialize all variables that we clean on shutdown */
  device = (SDL_VideoDevice*)SDL_malloc(sizeof(SDL_VideoDevice));
  if (device) {
    SDL_memset(device, 0, sizeof(SDL_VideoDevice));
    device->hidden = (struct SDL_PrivateVideoData*)SDL_malloc(
      sizeof(struct SDL_PrivateVideoData));
  }
  if ((device == NULL) || (device->hidden == NULL)) {
    SDL_OutOfMemory();
    if (device) {
      SDL_free(device);
    }
    return (0);
  }
  SDL_memset(device->hidden, 0, sizeof(struct SDL_PrivateVideoData));

  device->handles_any_size = 1;
  /* Set the function pointers */
  device->VideoInit = KMSDRM_VideoInit;
  device->ListModes = KMSDRM_ListModes;
  device->VideoModeOK = KMSDRM_VideoModeOK;
  device->SetVideoMode = KMSDRM_SetVideoMode;
  device->CreateYUVOverlay = NULL;
  device->SetColors = KMSDRM_SetColors;
  device->UpdateRects = KMSDRM_UpdateRects;
  device->VideoQuit = KMSDRM_VideoQuit;
  device->AllocHWSurface = KMSDRM_AllocHWSurface;
  device->CheckHWBlit = NULL;
  device->FillHWRect = NULL;
  device->SetHWColorKey = NULL;
  device->SetHWAlpha = NULL;
  device->LockHWSurface = KMSDRM_LockHWSurface;
  device->UnlockHWSurface = KMSDRM_UnlockHWSurface;
  device->FlipHWSurface = KMSDRM_FlipHWSurface;
  device->FreeHWSurface = KMSDRM_FreeHWSurface; // TODO:: Obvious
  device->SetCaption = NULL;
  device->SetIcon = NULL;
  device->IconifyWindow = NULL;
  device->GrabInput = NULL;
  device->GetWMInfo = NULL;
  device->InitOSKeymap = KMSDRM_InitOSKeymap;
  device->PumpEvents = KMSDRM_PumpEvents;

  device->free = KMSDRM_DeleteDevice;

  return device;
}

VideoBootStrap KMSDRM_bootstrap = { KMSDRM_DRIVER_NAME,
                                    "SDL kmsdrm video driver",
                                    KMSDRM_Available,
                                    KMSDRM_CreateDevice };
