
// SPDX-License-Identifier: LGPL-2.1-or-later
/*
 * Copyright (C) 2020 João H. Spies <johnnyonflame@hotmail.com>
 *
 * KMS/DRM video backend code for SDL1
 */

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <math.h>

#include "SDL_thread.h"
#include "SDL_video.h"
#include "SDL_kmsdrmvideo.h"
#include "SDL_kmsdrmmisc_c.h"

int KMSDRM_LookupVidMode(_THIS, int width, int height)
{
	for (int i = 0; i < drm_vid_mode_count; i++) {
		if (drm_vid_modes[i]->w == width && drm_vid_modes[i]->h == height) {
			return i;
		}
	}

	return -1;
}

void KMSDRM_RegisterVidMode(_THIS, int width, int height)
{
	if (KMSDRM_LookupVidMode(this, width, height) >= 0) {
		return;
	}

	drm_vid_mode_count++;
	drm_vid_modes = SDL_realloc(drm_vid_modes, sizeof(*drm_vid_modes) * (drm_vid_mode_count + 1));
	drm_vid_modes[drm_vid_mode_count] = NULL;
	drm_vid_modes[drm_vid_mode_count-1] = SDL_calloc(1, sizeof(**drm_vid_modes));
	drm_vid_modes[drm_vid_mode_count-1]->x = 0;
	drm_vid_modes[drm_vid_mode_count-1]->y = 0;
	drm_vid_modes[drm_vid_mode_count-1]->w = width;
	drm_vid_modes[drm_vid_mode_count-1]->h = height;
}
