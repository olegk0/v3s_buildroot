/***********************************************************************************
  Snes9x - Portable Super Nintendo Entertainment System (TM) emulator.

  See CREDITS file to find the copyright owners of this file.

  SDL Input/Audio/Video code (many lines of code come from snes9x & drnoksnes)
  (c) Copyright 2011         Makoto Sugano (makoto.sugano@gmail.com)

  Snes9x homepage: http://www.snes9x.com/

  Permission to use, copy, modify and/or distribute Snes9x in both binary
  and source form, for non-commercial purposes, is hereby granted without
  fee, providing that this license information and copyright notice appear
  with all copies and any derived work.

  This software is provided 'as-is', without any express or implied
  warranty. In no event shall the authors be held liable for any damages
  arising from the use of this software or it's derivatives.

  Snes9x is freeware for PERSONAL USE only. Commercial users should
  seek permission of the copyright holders first. Commercial use includes,
  but is not limited to, charging money for Snes9x or software derived from
  Snes9x, including Snes9x or derivatives in commercial game bundles, and/or
  using Snes9x as a promotion for your commercial product.

  The copyright holders request that bug fixes and improvements to the code
  should be forwarded to them so everyone can benefit from the modifications
  in future versions.

  Super NES and Super Nintendo Entertainment System are trademarks of
  Nintendo Co., Limited and its subsidiary companies.
 ***********************************************************************************/

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#include "snes9x.h"
#include "memmap.h"
#include "ppu.h"
#include "controls.h"
#include "movie.h"
#include "logger.h"
#include "conffile.h"
// #include "blit.h"
#include "display.h"

#include "sdl_snes9x.h"

struct GUIData
{
  SDL_Surface* sdl_screen;
  uint8* snes_buffer;
  // uint8* blit_screen;
  // uint32 blit_screen_pitch;
};
static struct GUIData GUI;

typedef void (*Blitter)(uint8*, int, uint8*, int, int, int);

#ifdef __linux
// Select seems to be broken in 2.x.x kernels - if a signal interrupts a
// select system call with a zero timeout, the select call is restarted but
// with an infinite timeout! The call will block until data arrives on the
// selected fd(s).
//
// The workaround is to stop the X library calling select in the first
// place! Replace XPending - which polls for data from the X server using
// select - with an ioctl call to poll for data and then only call the blocking
// XNextEvent if data is waiting.
#define SELECT_BROKEN_FOR_SIGNALS
#endif

static void
SetupImage(void);
static void
TakedownImage(void);

void
S9xExtraDisplayUsage(void)
{
}

void
S9xParseDisplayArg(char** argv, int& i, int argc)
{
}

const char*
S9xParseDisplayConfig(ConfigFile& conf, int pass)
{
  if (pass != 1)
    return ("Unix/SDL");
  return "";
}

static void
FatalError(const char* str)
{
  fprintf(stderr, "%s\n", str);
  SDL_Quit();
  S9xExit();
}

void
S9xInitDisplay(int argc, char** argv)
{
  GUI.snes_buffer = NULL;
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("Unable to initialize SDL: %s\n", SDL_GetError());
  }

  atexit(SDL_Quit);

  /*
   * domaemon
   *
   * we just go along with RGB565 for now, nothing else..
   */

  S9xSetRenderPixelFormat(RGB565);

  GUI.sdl_screen = SDL_SetVideoMode(SNES_WIDTH, SNES_HEIGHT, 16, 0);

  SDL_ShowCursor(SDL_DISABLE);
  if (GUI.sdl_screen == NULL) {
    printf("Unable to set video mode: %s\n", SDL_GetError());
    exit(1);
  }

  // S9xBlitFilterInit();
  /*
   * domaemon
   *
   * buffer allocation, quite important
   */
  SetupImage();
}

void
S9xDeinitDisplay(void)
{
  TakedownImage();
  // S9xBlitFilterDeinit();
  SDL_Quit();
}

static void
TakedownImage(void)
{
  if (GUI.snes_buffer) {
    free(GUI.snes_buffer);
    GUI.snes_buffer = NULL;
  }

  S9xGraphicsDeinit();
}

static void
SetupImage(void)
{
  TakedownImage();
  // domaemon: The whole unix code basically assumes output=(original * 2);
  // This way the code can handle the SNES filters, which does the 2X.
  GFX.Pitch = MAX_SNES_WIDTH * 2;
  GUI.snes_buffer =
    (uint8*)malloc(GFX.Pitch * ((SNES_HEIGHT_EXTENDED + 4) * 2));
  if (!GUI.snes_buffer)
    FatalError("Failed to allocate GUI.snes_buffer.");

  GFX.Screen = (uint16*)(GUI.snes_buffer);

  S9xGraphicsInit();
}

void
CheckRes(int width, int height)
{
  if (GUI.sdl_screen->w != width || GUI.sdl_screen->h != height) {
    // printf("h:%d  w:%d  \n", height, width);
    GUI.sdl_screen = SDL_SetVideoMode(width, height, 16, 0);
    if (!GUI.sdl_screen) {
      S9xDeinitDisplay();
      FatalError("Failed SDL_SetVideoMode.");
    }
  }
}

inline void
BlitPixSimple1x1(uint8* srcPtr,
                 int srcRowBytes,
                 uint8* dstPtr,
                 int dstRowBytes,
                 int width,
                 int height)
{
  width <<= 1;

  for (; height; height--) {
    memcpy(dstPtr, srcPtr, width);
    srcPtr += srcRowBytes;
    dstPtr += dstRowBytes;
  }
}

void
S9xPutImage(int width, int height)
{

  if (width <= SNES_WIDTH) {
    if (height > SNES_HEIGHT_EXTENDED) {
      CheckRes(width, SNES_HEIGHT_EXTENDED);
      // dp1=sp1 dp2=sp1 ...  = 2x width  S9xBlitPixSimple2x1
      BlitPixSimple1x1((uint8*)GFX.Screen,
                       GFX.Pitch,
                       (uint8*)GUI.sdl_screen->pixels,
                       GUI.sdl_screen->pitch,
                       GUI.sdl_screen->w,
                       GUI.sdl_screen->h);
    } else {
      CheckRes(width, height);
      // S9xBlitPixSimple2x2
      BlitPixSimple1x1((uint8*)GFX.Screen,
                       GFX.Pitch,
                       (uint8*)GUI.sdl_screen->pixels,
                       GUI.sdl_screen->pitch,
                       GUI.sdl_screen->w,
                       GUI.sdl_screen->h);
    }
  } else { // width > SNES_WIDTH
    if (height <= SNES_HEIGHT_EXTENDED) {
      CheckRes(SNES_WIDTH, height);
      // 1 line to 2  = 2x height  S9xBlitPixSimple1x2
      uint8* srcPtr = (uint8*)GFX.Screen;
      uint8* dstPtr = (uint8*)GUI.sdl_screen->pixels;
      for (int _height = 0; _height < GUI.sdl_screen->h; _height++) {
        for (int w = 0; w < GUI.sdl_screen->w; w++) {
          uint32 sw = ((uint32*)srcPtr)[w];
          ((uint16*)dstPtr)[w] = sw | (sw >> 16);
        }
        srcPtr += GFX.Pitch;
        dstPtr += GUI.sdl_screen->pitch;
      }
    } else {
      CheckRes(SNES_WIDTH, SNES_HEIGHT_EXTENDED);
      BlitPixSimple1x1((uint8*)GFX.Screen,
                       GFX.Pitch,
                       (uint8*)GUI.sdl_screen->pixels,
                       GUI.sdl_screen->pitch,
                       GUI.sdl_screen->w,
                       GUI.sdl_screen->h);
    }
  }

  SDL_Flip(GUI.sdl_screen);

  // prevWidth = width;
  // prevHeight = height;
}

void
S9xMessage(int type, int number, const char* message)
{
  const int max = 36 * 3;
  static char buffer[max + 1];

  fprintf(stdout, "%s\n", message);
  strncpy(buffer, message, max + 1);
  buffer[max] = 0;
  S9xSetInfoString(buffer);
}

const char*
S9xStringInput(const char* message)
{
  static char buffer[256];

  printf("%s: ", message);
  fflush(stdout);

  if (fgets(buffer, sizeof(buffer) - 2, stdin))
    return (buffer);

  return (NULL);
}

void
S9xSetTitle(const char* string)
{
  SDL_WM_SetCaption(string, string);
}

void
S9xSetPalette(void)
{
  return;
}
