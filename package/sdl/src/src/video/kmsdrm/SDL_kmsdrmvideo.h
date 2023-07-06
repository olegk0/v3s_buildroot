/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2012 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"
#include "SDL_stdinc.h"

#include <sys/types.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <drm_helper.h>

#ifndef _SDL_kmsdrmvideo_h
#define _SDL_kmsdrmvideo_h

#define ENABLE_KMSDRM_DEBUG 1
#ifdef ENABLE_KMSDRM_DEBUG
#define kmsdrm_dbg_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define kmsdrm_dbg_printf(fmt, ...)
#endif

#include "../SDL_sysvideo.h"

/* Default refresh rate. Can be set with the environment variable
 * SDL_VIDEO_REFRESHRATE */
#define KMSDRM_DEFAULT_REFRESHRATE 60

/* Hidden "this" pointer for the video functions */
#define _THIS SDL_VideoDevice* this

typedef struct drm_input_dev
{
  char* path;
  int fd;
  struct drm_input_dev* next;
} drm_input_dev;

struct SDL_PrivateVideoData
{
  SDL_Rect** vid_modes;
  int vid_mode_count;

  Uint32 size;
  Uint32 handle;
  // void *map;
  Uint8* shadow_buffer;
  // Uint32 *yuv_palette;

  Uint32 palette[256];
  // Uint16 palette[256];
  //  Uint32 drm_gamma_lut_blob_id;

  SDL_mutex* triplebuf_mutex;
  SDL_cond* triplebuf_cond;
  SDL_Thread* triplebuf_thread;
  int triplebuf_thread_stop;

  drm_input_dev *keyboards, *mice;

  int w, h, crtc_w, crtc_h;
  int bpp;

  // drm helper
  DhHwInfo* hw_info;
  DhPlaneInfo* plane_info;
  // con input
  int current_vt;
  int saved_vt;
  int keyboard_fd;
  int saved_kbd_mode;
  struct termios saved_kbd_termios;

  int mouse_fd;
#if SDL_INPUT_TSLIB
  struct tsdev* ts_dev;
#endif
  int switched_away;
};

#define drm_crtc_w (this->hidden->crtc_w)
#define drm_crtc_h (this->hidden->crtc_h)
#define drm_plane_info (this->hidden->plane_info)
#define drm_hw_info (this->hidden->hw_info)
#define drm_vid_modes (this->hidden->vid_modes)
#define drm_vid_mode_count (this->hidden->vid_mode_count)
#define drm_size (this->hidden->size)
#define drm_handle (this->hidden->handle)
#define drm_map (this->hidden->map)
#define drm_shadow_buffer (this->hidden->shadow_buffer)
#define drm_first_pipe (this->hidden->first_pipe)
#define drm_first_prop_store (this->hidden->first_prop_store)
#define drm_buffers (this->hidden->buffers)
#define drm_mode_blob_id (this->hidden->mode_blob_id)
#define drm_yuv_palette (this->hidden->yuv_palette)
#define drm_palette (this->hidden->palette)
#define drm_palette_blob_id (this->hidden->drm_gamma_lut_blob_id)
#define drm_req_destroy_dumb (this->hidden->req_destroy_dumb)
#define drm_req_create (this->hidden->req_create)
#define drm_req_map (this->hidden->req_map)
#define drm_triplebuf_mutex (this->hidden->triplebuf_mutex)
#define drm_triplebuf_cond (this->hidden->triplebuf_cond)
#define drm_triplebuf_thread (this->hidden->triplebuf_thread)
#define drm_triplebuf_thread_stop (this->hidden->triplebuf_thread_stop)

#define current_vt (this->hidden->current_vt)
#define saved_vt (this->hidden->saved_vt)
#define keyboard_fd (this->hidden->keyboard_fd)
#define saved_kbd_mode (this->hidden->saved_kbd_mode)
#define saved_kbd_termios (this->hidden->saved_kbd_termios)
#define mouse_fd (this->hidden->mouse_fd)
#if SDL_INPUT_TSLIB
#define ts_dev (this->hidden->ts_dev)
#endif
#define switched_away (this->hidden->switched_away)

#endif /* _SDL_kmsdrmvideo_h */
