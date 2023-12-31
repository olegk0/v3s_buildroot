/***************************************************************************
                          surface.cpp  -  description
                             -------------------
    begin                : Jun 13 2007
    copyright            : (C) 2007 by Giuseppe D'Aqui'
    email                : kumber@tiscalinet.it
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License, Version 2,      *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

#include "dephine.h"
#include "SDL/SDL.h"
#include "screen.h"
#include "surface.h"

void Surface::init(SDL_Surface *surface, Uint32 frame_size_x, Uint32 frame_size_y)
{
	SDL_Surface *m_surface = Screen::instance()->convert_surface_format(surface);

	if (m_surface == NULL) {
		DEBWARN("Image display format conversion failed: " << SDL_GetError() << "\n");
	}

	Uint32 num_frames_x, num_frames_y;

	if ((frame_size_x == 0) || (frame_size_y == 0)) {
		m_frame_size_x = m_surface->w;
		m_frame_size_y = m_surface->h;
		num_frames_x = 1;
		num_frames_y = 1;
	} else {
		m_frame_size_x = frame_size_x;
		m_frame_size_y = frame_size_y;

		num_frames_x = (m_surface->w) / frame_size_x;
		num_frames_y = (m_surface->h) / frame_size_y;
	}

	m_num_frames = num_frames_x * num_frames_y;

	m_surfaces = new Frame[m_num_frames];

	SDL_Rect src;

	for (Uint32 i = 0; i < num_frames_x; i++) {
		for (Uint32 k = 0; k < num_frames_y; k++) {
			src.x = i * m_frame_size_x;
			src.y = k * m_frame_size_y;

			src.w = m_frame_size_x;
			src.h = m_frame_size_y;

			SDL_Surface *surface_temp =
				SDL_CreateRGBSurface(0, m_frame_size_x, m_frame_size_y, Screen::instance()->get_bpp(), 0x00FF0000,
						     0x0000FF00, 0x000000FF, 0xFF000000);

			if (surface_temp == NULL) {
				DEBOUT("CreateRGBSurface failed: " << SDL_GetError());
				exit(1);
			}

			SDL_BlitSurface(m_surface, &src, surface_temp, NULL);

			m_surfaces[k * num_frames_x + i].image = surface_temp;

			//Set pink as transparent color
			SDL_SetColorKey(m_surfaces[k * num_frames_x + i].image, SDL_TRUE,
					SDL_MapRGB(m_surfaces[k * num_frames_x + i].image->format, 255, 0, 255));

			m_surfaces[k * num_frames_x + i].pause = 1;
		}
	}

	m_is_inited = true;
}

void Surface::put_screen(ScreenCoord scr_coord, int frame_no)
{
	Screen *screen = Screen::instance();

	if (frame_no == -1) {
		screen->blit_surface(m_surfaces[0].image, NULL, scr_coord);

	} else {
		SDL_Rect src;
		src.x = 0;
		src.y = 0;

		src.w = m_frame_size_x;
		src.h = m_frame_size_y;

		screen->blit_surface(m_surfaces[frame_no].image, &src, scr_coord);
	}
}

Surface::~Surface()
{
	if (m_is_inited) {
		for (Uint32 i = 0; i < m_num_frames; i++) {
			SDL_FreeSurface(m_surfaces[i].image);
		}
	}
}
