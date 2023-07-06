/* Copyright (c) 2013-2015 Jeffrey Pfau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "main.h"

#include <mgba/core/core.h>
#include <mgba/core/thread.h>
#include <mgba/core/version.h>

static bool mSDLSWInit(struct mSDLRenderer* renderer);
static void mSDLSWRunloop(struct mSDLRenderer* renderer, void* user);
static void mSDLSWDeinit(struct mSDLRenderer* renderer);
static SDL_Surface* surface = NULL;
static size_t stride;

void mSDLSWCreate(struct mSDLRenderer* renderer) {
	renderer->init = mSDLSWInit;
	renderer->deinit = mSDLSWDeinit;
	renderer->runloop = mSDLSWRunloop;
}

bool mSDLSWInit(struct mSDLRenderer* renderer) {
#ifdef COLOR_16_BIT
	surface = SDL_SetVideoMode(renderer->viewportWidth, renderer->viewportHeight, 16,
	                           SDL_HWSURFACE |
	                               /*SDL_DOUBLEBUF | */ (SDL_FULLSCREEN * renderer->player.fullscreen));
#else
	surface = SDL_SetVideoMode(renderer->viewportWidth, renderer->viewportHeight, 32,
	                           SDL_HWSURFACE |
	                               /*SDL_DOUBLEBUF | */ (SDL_FULLSCREEN * renderer->player.fullscreen));
#endif
	SDL_WM_SetCaption(projectName, "");

	unsigned width, height;
	renderer->core->desiredVideoDimensions(renderer->core, &width, &height);
	//	SDL_LockSurface(surface);

	stride = surface->pitch / BYTES_PER_PIXEL;
	if (renderer->ratio == 1) {
		renderer->core->setVideoBuffer(renderer->core, surface->pixels, stride);
	} else {
		return false;
	}

	return true;
}

void FlipFrame(void) {
	SDL_Flip(surface);
}

void mSDLSWRunloop(struct mSDLRenderer* renderer, void* user) {
	struct mCoreThread* context = user;
	SDL_Event event;
	while (mCoreThreadIsActive(context)) {
		while (SDL_PollEvent(&event)) {
			mSDLHandleEvent(context, &renderer->player, &event);
		}
#if 0
		if (mCoreSyncWaitFrameStart(&context->impl->sync)) {
			/*
			            switch (renderer->ratio) {
			            case 1:
			                break;
			            default:
			                abort();
			            }
			            */
			//			SDL_UnlockSurface(surface);
			SDL_Flip(surface);
			//			SDL_LockSurface(surface);
			//			renderer->core->setVideoBuffer(renderer->core, surface->pixels, stride);
		}
		mCoreSyncWaitFrameEnd(&context->impl->sync);
#else
		//	SDL_Flip(surface);
		usleep(1000);
#endif
	}
}

void mSDLSWDeinit(struct mSDLRenderer* renderer) {
	if (renderer->ratio > 1) {
	}
	SDL_UnlockSurface(surface);
}
