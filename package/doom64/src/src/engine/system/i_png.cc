// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
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
// DESCRIPTION:
//    PNG image system
//
//-----------------------------------------------------------------------------

#include <algorithm>
#include <math.h>

#include "doomdef.h"
#include "i_swap.h"
#include "z_zone.h"
#include "gl_texture.h"

#include <imp/Image>
#include <sstream>
#include <imp/Property>
#include <imp/Wad>

FloatProperty i_gamma("i_gamma", "", 0.0f, 0,
                      [](const FloatProperty&, float, float&)
                      {
                          GL_DumpTextures();
                      });

//
// I_GetRGBGamma
//

d_inline static byte I_GetRGBGamma(int c) {
    return (byte)MIN(pow((float)c, (1.0f + (0.01f * i_gamma))), 255);
}

//
// I_TranslatePalette
// Increases the palette RGB based on gamma settings
//

static void I_TranslatePalette(gfx::Palette &dest) {
    if (i_gamma == 0)
        return;

    auto color_count = dest.count();
    auto color_size = dest.traits().bytes;

    for(size_t i = 0; i < color_count; i += color_size) {
        dest.data_ptr()[i + 0] = I_GetRGBGamma(dest.data_ptr()[i + 0]);
        dest.data_ptr()[i + 1] = I_GetRGBGamma(dest.data_ptr()[i + 1]);
        dest.data_ptr()[i + 2] = I_GetRGBGamma(dest.data_ptr()[i + 2]);
    }
}

gfx::Image I_ReadImage(int lump, dboolean palette, dboolean nopack, double alpha, int palindex)
{
    // get lump data
    auto l = wad::find(lump);

    auto image = l->as_image();

    if (palindex && image.is_indexed())
    {
        auto pal = image.palette();

        char palname[9];
        snprintf(palname, sizeof(palname), "PAL%4.4s%d", l->lump_name().data(), palindex);

        gfx::Palette newpal;
        if (auto pl = wad::find(palname))
        {
            auto bytes = pl->as_bytes();
            gfx::Rgb *pallump = reinterpret_cast<gfx::Rgb *>(&bytes[0]);
            newpal = *pal;

            // swap out current palette with the new one
            for (auto& c : newpal.map<gfx::Rgba>()) {
                c = gfx::Rgba { pallump->red, pallump->green, pallump->blue, c.alpha };
                pallump++;
            }
        } else {
            newpal = gfx::Palette { pal->format(), 16, pal->data_ptr() + (16 * palindex) * pal->traits().bytes };
        }

        I_TranslatePalette(newpal);
        image.set_palette(newpal);
    }

    if (!palette)
        image.convert(alpha ? gfx::PixelFormat::rgba : gfx::PixelFormat::rgb);

    return image;
}

//
// I_PNGReadData
//
void *I_PNGReadData(int lump, dboolean palette, dboolean nopack, dboolean alpha,
                    int* w, int* h, int* offset, int palindex)
{
    auto image = I_ReadImage(lump, palette, nopack, alpha, palindex);

    // look for offset chunk if specified
    if(offset) {
        offset[0] = image.offsets().x;
        offset[1] = image.offsets().y;
    }

    if(w) {
        *w = image.width();
    }
    if(h) {
        *h = image.height();
    }

    auto length = image.traits().bytes * image.width() * image.height();
    auto retval = reinterpret_cast<byte*>(malloc(length));
    std::copy_n(image.data_ptr(), length, retval);

    return retval;
}
