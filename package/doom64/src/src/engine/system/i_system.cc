// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// Copyright(C) 1993-1997 Id Software, Inc.
// Copyright(C) 2007-2012 Samuel Villarreal
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION: System Interface
//
//-----------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#endif

#include <stdarg.h>
#include <sys/stat.h>
#include "doomstat.h"
#include "doomdef.h"
#include "m_misc.h"
#include "i_video.h"
#include "d_net.h"
#include "g_demo.h"
#include "d_main.h"
#include "con_console.h"
#include "z_zone.h"
#include "i_system.h"
#include "i_audio.h"
#include "gl_draw.h"

BoolProperty i_interpolateframes("i_interpolateframes", "", false);

ticcmd_t        emptycmd;

//
// I_uSleep
//

void I_Sleep(unsigned long usecs) {
    SDL_Delay(usecs);
}

static int basetime = 0;

//
// I_GetTimeNormal
//

static int I_GetTimeNormal(void) {
    uint32 ticks;

    ticks = SDL_GetTicks();

    if(basetime == 0) {
        basetime = ticks;
    }

    ticks -= basetime;

    return (ticks * TICRATE) / 1000;
}

//
// I_GetTime_Error
//

static int I_GetTime_Error(void) {
    I_Error("I_GetTime_Error: GetTime() used before initialization");
    return 0;
}

//
// I_InitClockRate
//

void I_InitClockRate(void) {
    I_GetTime = I_GetTimeNormal;
}

//
// FRAME INTERPOLTATION
//

static unsigned int start_displaytime;
static unsigned int displaytime;
static dboolean InDisplay = false;

dboolean realframe = false;

fixed_t         rendertic_frac = 0;
unsigned int    rendertic_start;
unsigned int    rendertic_step;
unsigned int    rendertic_next;
const float     rendertic_msec = 100 * TICRATE / 100000.0f;

//
// I_StartDisplay
//

dboolean I_StartDisplay(void) {
    rendertic_frac = I_GetTimeFrac();

    if(InDisplay) {
        return false;
    }

    start_displaytime = SDL_GetTicks();
    InDisplay = true;

    return true;
}

//
// I_EndDisplay
//

void I_EndDisplay(void) {
    displaytime = SDL_GetTicks() - start_displaytime;
    InDisplay = false;
}

//
// I_GetTimeFrac
//

fixed_t I_GetTimeFrac(void) {
    unsigned long now;
    fixed_t frac;

    now = SDL_GetTicks();

    if(rendertic_step == 0) {
        return FRACUNIT;
    }
    else {
        frac = (fixed_t)((now - rendertic_start + displaytime) * FRACUNIT / rendertic_step);
        if(frac < 0) {
            frac = 0;
        }
        if(frac > FRACUNIT) {
            frac = FRACUNIT;
        }
        return frac;
    }
}

//
// I_GetTime_SaveMS
//

void I_GetTime_SaveMS(void) {
    rendertic_start = SDL_GetTicks();
    rendertic_next = (unsigned int)((rendertic_start * rendertic_msec + 1.0f) / rendertic_msec);
    rendertic_step = rendertic_next - rendertic_start;
}

//
// I_BaseTiccmd
//

ticcmd_t* I_BaseTiccmd(void) {
    return &emptycmd;
}

/**
 * @brief Get the user-writeable directory.
 *
 * Assume this is the only user-writeable directory on the system.
 *
 * @return Fully-qualified path that ends with a separator or NULL if not found.
 * @note The returning value MUST be freed by the caller.
 */

char *I_GetUserDir(void) {
#ifdef _WIN32
    return I_GetBaseDir();
#else
    return SDL_GetPrefPath("", "doom64ex");
#endif
}

/**
 * @brief Get the directory which contains this program.
 *
 * @return Fully-qualified path that ends with a separator or NULL if not found.
 * @note The returning value MUST be freed by the caller.
 */

char *I_GetBaseDir(void) {
    return SDL_GetBasePath();
}

/**
 * @brief Find a regular file in the user-writeable directory.
 *
 * @return Fully-qualified path or NULL if not found.
 * @note The returning value MUST be freed by the caller.
 */

char *I_GetUserFile(const char *file) {
    char *path;
    char *userdir;

    if (!(userdir = I_GetUserDir()))
        return NULL;

    path = (char*) malloc(512);

    snprintf(path, 511, "%s%s", userdir, file);
    
    return path;
}

//
// I_GetTime
// returns time in 1/70th second tics
// now 1/35th?
//

int (*I_GetTime)(void) = I_GetTime_Error;

//
// I_GetTimeMS
//
// Same as I_GetTime, but returns time in milliseconds
//

int I_GetTimeMS(void) {
    uint32 ticks;

    ticks = SDL_GetTicks();

    if(basetime == 0) {
        basetime = ticks;
    }

    return ticks - basetime;
}

//
// I_GetRandomTimeSeed
//

unsigned long I_GetRandomTimeSeed(void) {
    // not exactly random....
    return SDL_GetTicks();
}

//
// I_Init
//

void I_Init(void) {
#ifdef USESYSCONSOLE
    //I_SpawnLauncher(hwndMain);
#endif

    I_InitVideo();
    I_InitClockRate();
}

//
// I_Error
//

void I_Error(const char* string, ...) {
    char buff[1024];
    va_list    va;

    I_ShutdownSound();

    va_start(va, string);
    vsprintf(buff, string, va);
    va_end(va);

    fprintf(stderr, "Error - %s\n", buff);
    fflush(stderr);

    I_Printf("\n********* ERROR *********\n");
    I_Printf("%s", buff);

    if(usingGL) {
        while(1) {
            GL_ClearView(0xFF000000);
            Draw_Text(0, 0, WHITE, 1, 1, "Error - %s\n", buff);
            GL_SwapBuffers();

            if(I_ShutdownWait()) {
                break;
            }

            I_Sleep(1);
        }
    }
    else {
        I_ShutdownVideo();
    }

    // TODO: Show sysconsole and quit

    exit(0);    // just in case...
}

//
// I_Quit
//

void I_Quit(void) {
    if(demorecording) {
        endDemo = true;
        G_CheckDemoStatus();
    }

    M_SaveDefaults();

#ifdef USESYSCONSOLE
    // I_DestroySysConsole();
#endif

    I_ShutdownSound();
    I_ShutdownVideo();

    exit(0);
}

//
// I_Printf
//

void I_Printf(const char* string, ...) {
    char buff[1024];
    va_list    va;

    dmemset(buff, 0, 1024);

    va_start(va, string);
    vsprintf(buff, string, va);
    va_end(va);

// #ifdef USESYSCONSOLE
//     {
//         char winBuff[1024];
//         char *c = buff;
//         char *b = winBuff;

//         dmemset(winBuff, 0, 1024);

//         do {
//             if(!*c) {
//                 break;
//             }

//             if(*c == '\n') {
//                 *b = '\r';
//                 b++;
//             }
//             *b = *c;
//             b++;

//         }
//         while(c++);

//         SendMessage(hwndBuffer, EM_LINESCROLL, 0, 0xffff);
//         SendMessage(hwndBuffer, EM_SCROLLCARET, 0, 0);
//         SendMessage(hwndBuffer, EM_REPLACESEL, 0, (LPARAM)winBuff);
//     }
// #endif

    printf("%s", buff);
    if(console_initialized) {
        CON_AddText(buff);
    }
}

//
// I_BeginRead
//

dboolean    inshowbusy=false;

void I_BeginRead(void) {
    if(!devparm) {
        return;
    }

    if(inshowbusy) {
        return;
    }
    inshowbusy=true;
    inshowbusy=false;
    BusyDisk=true;
}

//
// I_RegisterCvars
//

#ifdef _USE_XINPUT
CVAR_EXTERNAL(i_rsticksensitivity);
CVAR_EXTERNAL(i_rstickthreshold);
CVAR_EXTERNAL(i_xinputscheme);
#endif

void I_RegisterCvars(void) {
#ifdef _USE_XINPUT
    CON_CvarRegister(&i_rsticksensitivity);
    CON_CvarRegister(&i_rstickthreshold);
    CON_CvarRegister(&i_xinputscheme);
#endif
}

