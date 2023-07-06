/* vim:expandtab:ts=2 sw=2:
*/
/*  Grafx2 - The Ultimate 256-color bitmap paint program

	Copyright owned by various GrafX2 authors, see COPYRIGHT.txt for details.

    Grafx2 is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; version 2
    of the License.

    Grafx2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Grafx2; if not, see <http://www.gnu.org/licenses/>
*/

///@file 2gsformats.c
/// Formats for the Apple II GS
/// Support APF = Apple Preferred Format

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "global.h"
#include "fileformats.h"
#include "loadsavefuncs.h"
#include "io.h"
#include "gfx2mem.h"
#include "gfx2log.h"
#include "packbytes.h"

/**
 * Test for an Apple 2 GS picture file
 */
void Test_2GS(T_IO_Context * context, FILE * file)
{
  unsigned long file_size;
  dword chunksize;
  byte header[5];

  (void)context;
  File_error = 1;
  file_size = File_length_file(file);
  if (!Read_dword_le(file, &chunksize))
    return;
  if (!Read_bytes(file, header, sizeof(header)))
    return;
  if (0 != memcmp(header, "\x04MAIN", 5))
    return;
  if (file_size < chunksize)
    return;
  File_error = 0;
}

/**
 * Load a 16 entry Apple II GS palette from file
 */
static int Load_2GS_Palette(T_Components * palette, FILE * file)
{
  unsigned int i;
  for (i = 0; i < 16; i++)
  {
    word palentry;
    if (!Read_word_le(file, &palentry))
      return 0;
    palette[i].R = ((palentry >> 8) & 0x0f) * 0x11;
    palette[i].G = ((palentry >> 4) & 0x0f) * 0x11;
    palette[i].B = (palentry & 0x0f) * 0x11;
  }
  return 1;
}

#define COMPONENTS(x) linepal[x].R, linepal[x].G, linepal[x].B

/**
 * set the (2 or 4) pixels for the byte.
 */
#define WRITE2GS_PIXELS \
if (multipalcount > 0)  \
{                       \
  if (linemode & 0x80)  \
  {                     \
    Set_pixel_24b(context, x++, y, COMPONENTS(8 + (pixel >> 6)));        \
    Set_pixel_24b(context, x++, y, COMPONENTS(12 + ((pixel >> 4) & 3))); \
    Set_pixel_24b(context, x++, y, COMPONENTS((pixel >> 2) & 3));        \
    Set_pixel_24b(context, x++, y, COMPONENTS(4 + (pixel & 3)));         \
  }                     \
  else                  \
  {                     \
    Set_pixel_24b(context, x++, y, COMPONENTS(pixel >> 4));       \
    Set_pixel_24b(context, x++, y, COMPONENTS(pixel & 15));       \
  }                     \
}                       \
else                    \
{                       \
  if (linemode & 0x80)  \
  {                     \
    Set_pixel(context, x++, y, (linemode << 4) | 8 | (pixel >> 6));          \
    Set_pixel(context, x++, y, (linemode << 4) | 12 | ((pixel >> 4) & 3));   \
    Set_pixel(context, x++, y, (linemode << 4) | ((pixel >> 2) & 3));        \
    Set_pixel(context, x++, y, (linemode << 4) | 4 | (pixel & 3));           \
  }                     \
  else                  \
  {                     \
    Set_pixel(context, x++, y, (linemode << 4) | (pixel >> 4));          \
    Set_pixel(context, x++, y, (linemode << 4) | (pixel & 15));          \
  }                     \
}

void Load_2GS(T_IO_Context * context)
{
  FILE * file;
  unsigned long file_size;
  dword chunksize;
  byte header[5];
  word mode;
  word width;
  word height;
  word palcount;
  word palindex;
  long lineoffset;
  long dataoffset;
  word y;
  byte bpp = 2;
  word multipalcount = 0;
  long multipaloffset = -1;
  long offset;

  File_error = 1;
  file = Open_file_read(context);
  if (file == NULL)
    return;
  file_size = File_length_file(file);
  if (!Read_dword_le(file, &chunksize))
    return;
  if (!Read_bytes(file, header, sizeof(header)))
    goto error;
  if (0 != memcmp(header, "\x04MAIN", 5))
    goto error;
  if (!Read_word_le(file, &mode) || !Read_word_le(file, &width) || !Read_word_le(file, &palcount))
    goto error;

  if (Config.Clear_palette)
    memset(context->Palette, 0, sizeof(T_Palette));
  for (palindex = 0; palindex < palcount && palindex < 16; palindex++)
  {
    if (!Load_2GS_Palette(context->Palette + (palindex * 16), file))
      goto error;
  }
  if (palindex < palcount)
  {
    GFX2_Log(GFX2_WARNING, "2GS %u palettes, skipping %u of them !\n",
             (unsigned)palcount, (unsigned)(palcount - palindex));
    fseek(file, 16*2*(palcount - palindex), SEEK_CUR);
  }
  if (!Read_word_le(file, &height))
    goto error;
  lineoffset = ftell(file);
  dataoffset = lineoffset + 4 * (long)height;
  GFX2_Log(GFX2_DEBUG, "2GS mode %02x %ux%u %u palette(s)\n", mode, (unsigned)width, (unsigned)height, (unsigned)palcount);
  if (palcount > 16)
    palcount = 16;

  // read other chunks before decoding the picture
  GFX2_Log(GFX2_DEBUG, "2GS file_size : %06lx, chunksize : %06lx, current offset : %06lx\n", file_size, chunksize, dataoffset);
  offset = chunksize;
  while (chunksize > 0 && offset < (long)file_size)
  {
    char * p;
    byte c;
    fseek(file, offset, SEEK_SET);
    if (!Read_dword_le(file, &chunksize) || !Read_byte(file, &c))
      goto error;
    p = GFX2_malloc(c + 1);
    if (p)
    {
      fread(p, 1, c, file);
      p[c] = '\0';
      GFX2_Log(GFX2_DEBUG, "%06lx: %04x %s\n", offset, chunksize, p);
      if (c == 4 && 0 == memcmp(p, "NOTE", 4))
      {
        word len;
        if (Read_word_le(file, &len))
        {
          free(p);
          p = GFX2_malloc(len + 1);
          if (p)
          {
            fread(p, 1, len, file);
            p[len] = '\0';
            GFX2_Log(GFX2_DEBUG, "  \"%s\"\n", p);
            strncpy(context->Comment, p, COMMENT_SIZE);
            context->Comment[COMMENT_SIZE] = '\0';
          }
        }
      }
      else if (c == 12 && 0 == memcmp(p, "SuperConvert", 12))
      {
        dword len = chunksize - 4 - 13;
        free(p);
        p = GFX2_malloc(len);
        if (p)
        {
          fread(p, 1, len, file);
          GFX2_LogHexDump(GFX2_DEBUG, "", (byte *)p, 0 /*offset + 4 + 13*/, (long)len);
        }
      }
      else if (c == 8 && 0 == memcmp(p, "MULTIPAL", 8))
      {
        if (Read_word_le(file, &multipalcount))
        {
          // all palettes are there...
          multipaloffset = ftell(file);
          GFX2_Log(GFX2_DEBUG, "2GS MULTIPAL count %u offset %06lx\n",
                   (unsigned)multipalcount, multipaloffset);
        }
      }
      free(p);
    }
    offset += chunksize;
  }

  if (multipalcount > 0)
  {
    bpp = 12;
  }
  else
  {
    while ((1 << bpp) < ((mode & 0x80) ? 4 : 16) * palcount)
      bpp++;
  }
  File_error = 0;
  Pre_load(context, width, height, file_size, FORMAT_2GS, (mode & 0x80) ? PIXEL_TALL : PIXEL_SIMPLE, bpp);
  if (File_error)
    goto error;
  for (y = 0; y < height; y++)
  {
    word linebytes; /* packed bytes for this scanline */
    byte linemode;  /* Scanline Control Byte :  rif0pppp
                                                |||   |_ palette index
                                                |||_____ fill mode 0/1
                                                ||______ interrupt enabled 0/1
                                                |_______ 0 : 320px, 1 : 640px  */
    word x;
    T_Components linepal[16];
    if (multipalcount > y)
    {
      fseek(file, multipaloffset + 32 * y, SEEK_SET);
      if (!Load_2GS_Palette(linepal, file))
        goto error;
    }
    fseek(file, lineoffset, SEEK_SET);
    if (!Read_word_le(file, &linebytes) || !Read_byte(file, &linemode))
      goto error;
    //GFX2_Log(GFX2_DEBUG, "Line #%03u mode/pal=%02x %u bytes\n", (unsigned)y, linemode, (unsigned)linebytes);
    lineoffset += 4;
    fseek(file, dataoffset, SEEK_SET);
    if (multipalcount > 0)
    {
      linemode = 0;
    }
    for (x = 0; x < width; )
    {
      byte code, pixel;
      unsigned count;
      if (!Read_byte(file, &code))
        goto error;
      count = (code & 0x3f) + 1;
      switch (code >> 6)
      {
      case 0: // unpacked
        while (count-- > 0)
        {
          if (!Read_byte(file, &pixel))
            goto error;
          WRITE2GS_PIXELS
        }
        break;
      case 3: // byte packed x 4
        count <<= 2;
#if defined(__GNUC__) && (__GNUC__ >= 7)
        __attribute__ ((fallthrough));
#endif
      case 1: // byte packed
        if (!Read_byte(file, &pixel))
          goto error;
        while (count-- > 0)
        {
          WRITE2GS_PIXELS
        }
        break;
      case 2: // dword packed
        {
          byte pixels[4];
          if (!Read_bytes(file, pixels, 4))
            goto error;
          while (count-- > 0)
          {
            int i;
            for (i = 0; i < 4; i++)
            {
              pixel = pixels[i];
              WRITE2GS_PIXELS
            }
          }
        }
        break;
      }
    }
    dataoffset += linebytes;
  }

  fclose(file);
  return;

error:
  File_error = 1;
  fclose(file);
}

/**
 * Save a 16 entry Apple II GS palette to file
 */
static int Save_2GS_Palette_Entry(FILE * file, const T_Components * palette)
{
  word palentry = ((palette->R << 4) & 0xf00) | (palette->G & 0xf0) | (palette->B >> 4);
  return Write_word_le(file, palentry);
}

struct line_data {
  byte used_colors[256 / 8];  // bit map of used colors
  word color_count;
  word palette_index;
};

void Save_2GS(T_IO_Context * context)
{
  FILE *file;
  word x, y, n;
  byte * palette_mapping;
  byte * palette_used_colors;
  struct line_data * lines;
  word palette_count = 0;
  long linelist_offset;

  File_error = 1;
  palette_mapping = GFX2_malloc(16 * context->Height);
  memset(palette_mapping, 0, 16 * context->Height);
  lines = GFX2_malloc(sizeof(struct line_data) * context->Height);
  palette_used_colors = GFX2_malloc((256 / 8) * context->Height);
  // Parcours de chaque ligne pour construire la / les palette(s)
  for (y = 0; y < context->Height; y++)
  {
    memset(lines + y, 0, sizeof(struct line_data));
    for (x = 0; x < context->Width; x++)
    {
      byte c = Get_pixel(context, x, y);
      if (!(lines[y].used_colors[c >> 3] & (1 << (c & 7))))
      {
        lines[y].used_colors[c >> 3] |= (1 << (c & 7));
        lines[y].color_count++;
      }
    }
    if (lines[y].color_count > 16)
    {
      GFX2_Log(GFX2_DEBUG, "%u colors used on line %u !\n", lines[y].color_count, (unsigned)y);
      free(palette_used_colors);
      free(palette_mapping);
      free(lines);
      return;
    }
    lines[y].palette_index = 0xffff;  // pas de palette attribuée
  }
  // attribue les palettes en commençant par celles qui ont le plus de couleurs
  for (n = 16; n > 0; n--)
  {
    for (y = 0 ; y < context->Height; y++)
    {
      if (lines[y].color_count == n)
      {
        word i;
        for (i = 0; i < palette_count; i++)
        {
          for (x = 0; x < 32; x++)
          {
            if (lines[y].used_colors[x] & ~palette_used_colors[i * (256 / 8) + x])
              break;
          }
          if (x == 32)
          {
            lines[y].palette_index = i;
            GFX2_Log(GFX2_DEBUG, "  line#%u => pal#%u\n", y, lines[y].palette_index);
            break;
          }
        }
        // évolution : chercher parmi les palettes avec moins de 16 couleurs s'il y en a une qui convient
        if (lines[y].palette_index == 0xffff)
        {
          for (i = 0; i < palette_count; i++)
          {
            word freeslots = 0;
            while (freeslots <= 15 && palette_mapping[i * 16 + 15 - freeslots] == 0)
              freeslots++;
            if (freeslots > 0)
            {
              word to_add_count = 0;
              byte to_add[16];
              for (x = 0; x < 256; x++)
                if ((lines[y].used_colors[x >> 3] & ~palette_used_colors[i * (256 / 8) + (x >> 3)]) & (1 << (x & 7)))
                {
                  // color used on the line but not in the palette
                  to_add[to_add_count++] = (byte)x;
                }
              if (to_add_count <= freeslots)
              {
                GFX2_Log(GFX2_DEBUG, "  add %u colors from line#%u to pal#%u\n",
                         (unsigned)to_add_count, (unsigned)y, (unsigned)i);
                lines[y].palette_index = i;
                for (x = 0; x < to_add_count; x++)
                {
                  // add the color to the palette
                  palette_used_colors[i * (256 / 8) + (to_add[x] >> 3)] |= (1 << (to_add[x] & 7));
                  palette_mapping[i * 16 + 16 - freeslots + x] = to_add[x];
                }
                break;
              }
            }
          }
          if (lines[y].palette_index == 0xffff)
          {
            word index;
            lines[y].palette_index = palette_count;
            for (x = 0, index = 0; x < 256; x++)
              if (lines[y].used_colors[x >> 3] & (1 << (x & 7)))
                palette_mapping[palette_count * 16 + index++] = x;
            memcpy(palette_used_colors + palette_count * (256 / 8), lines[y].used_colors, 256 / 8);
            GFX2_Log(GFX2_DEBUG, "  line#%u => pal#%u (new, %ucolors)\n",
                     (unsigned)y, (unsigned)palette_count, (unsigned)index);
            palette_count++;
          }
        }
      }
    }
  }
  GFX2_Log(GFX2_DEBUG, "%u palette(s) (16 colors each).\n", (unsigned)palette_count);
  // packing
  file = Open_file_write(context);
  if (file != NULL)
  {
    unsigned int i;
    byte pal_mapping[256] = { 0 };
    // Write Header
    if(!Write_dword_le(file, 0))  // chunksize, to be written afterward
      goto error;
    if(!Write_byte(file, 4) || !Write_bytes(file, "MAIN", 4))
      goto error;
    if (!Write_word_le(file, 0/*mode*/) || !Write_word_le(file, context->Width))
      goto error;
    // Write Palettes
    if (palette_count <= 16)
    {
      if (!Write_word_le(file, palette_count))
        goto error;
      for (i = 0; i < palette_count * 16; i++)
      {
        if (!Save_2GS_Palette_Entry(file, context->Palette + palette_mapping[i]))
          goto error;
      }
    }
    else
    {
      if (!Write_word_le(file, 1))
        goto error;
      for (i = 0; i < 16; i++)
      {
        if (!Save_2GS_Palette_Entry(file, context->Palette + palette_mapping[i]))
          goto error;
      }
    }
    GFX2_Log(GFX2_DEBUG, "%d 0x%x\n", (int)ftell(file), (int)ftell(file));
    // Write lines
    if (!Write_word_le(file, context->Height))
      goto error;
    linelist_offset = ftell(file);
    for (y = 0; y < context->Height; y++)
    {
      if (!Write_word_le(file, 0/*linebytes*/) || !Write_word_le(file, palette_count <= 16 ? lines[y].palette_index : 0))
        goto error;
    }
    GFX2_Log(GFX2_DEBUG, "  %d 0x%x\n", (int)ftell(file), (int)ftell(file));
    // Write Pixels
    for (y = 0; y < context->Height; y++)
    {
      byte b;
      T_PackBytes_data data;
      unsigned packed_len;
      for (i = 0; i < 16; i++)
      {
        pal_mapping[palette_mapping[16 * lines[y].palette_index + i]] = i;
      }
      PackBytes_pack_init(&data, file);
      for (x = 0; x < context->Width; x += 2)
      {
        b = pal_mapping[Get_pixel(context, x, y)] << 4;
        b |= pal_mapping[Get_pixel(context, x + 1, y)];
        if (PackBytes_pack_add(&data, b) < 0)
          goto error;
      }
      packed_len = PackBytes_pack_flush(&data);
      //fseek(file, 15 + 32 * palette_count + 2 + y * 4, SEEK_SET);
      fseek(file, linelist_offset + y * 4, SEEK_SET);
      if (!Write_word_le(file, packed_len))
        goto error;
      fseek(file, 0, SEEK_END);
    }
    {
      long chunk_size = ftell(file);
      fseek(file, 0, SEEK_SET);
      if (!Write_dword_le(file, (dword)chunk_size))
        goto error;
      fseek(file, 0, SEEK_END);
    }
    // Comment => NOTE
    if (palette_count > 16)
    {
      if (!Write_dword_le(file, context->Height * 32 + 9 + 2 + 4))
        goto error;
      if (!Write_bytes(file, "\x08MULTIPAL", 9))
        goto error;
      if (!Write_word_le(file, context->Height))
        goto error;
      for (y = 0; y < (unsigned)context->Height; y++)
      {
        for (i = 0; i < 16; i++)
        {
          if (!Save_2GS_Palette_Entry(file, context->Palette + palette_mapping[lines[y].palette_index * 16 + i]))
            goto error;
        }
      }
    }
    File_error = 0;
error:
    fclose(file);
  }
  free(palette_used_colors);
  free(lines);
  free(palette_mapping);
}
