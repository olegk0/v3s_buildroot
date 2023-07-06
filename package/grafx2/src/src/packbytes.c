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

///@file packbytes.c
/// Packbytes compression as used in Apple Preferred Format

#include <stdio.h>
#include <string.h>
#include "struct.h"
#include "io.h"
#include "gfx2log.h"
#include "packbytes.h"

void PackBytes_pack_init(T_PackBytes_data * data, FILE * f)
{
  memset(data, 0, sizeof(T_PackBytes_data));
  data->f = f;
}

int PackBytes_pack_add(T_PackBytes_data * data, byte b)
{
  switch (data->list_size)
  {
    case 0 : // First color
      data->list[0] = b;
      data->list_size = 1;
      break;
    case 1 : // second color
      data->repetition_mode = (data->list[0] == b);
      data->list[1] = b;
      data->list_size = 2;
      break;
    default: // next colors
      if (data->list[data->list_size - 1] == b)  // repeat detected
      {
        if ( !data->repetition_mode && data->list_size >= 63)
        {
          // diff mode with 126 bytes then 2 identical bytes
          data->list_size--;
          if (PackBytes_pack_flush(data) < 0)
            return -1;
          data->list[0] = b;
          data->list[1] = b;
          data->list_size = 2;
          data->repetition_mode = 1;
        }
        else if ((data->repetition_mode) || (data->list[data->list_size - 2] != b))
        {
          // same mode is kept
          if (data->list_size == 64)
          {
            if (PackBytes_pack_flush(data) < 0)
              return -1;
          }
          data->list[data->list_size++] = b;
        }
        else
        {
          // diff mode and 3 identical bytes
          data->list_size -= 2;
          if (PackBytes_pack_flush(data) < 0)
            return -1;
          data->list[0] = b;
          data->list[1] = b;
          data->list[2] = b;
          data->list_size = 3;
          data->repetition_mode = 1;
        }
      }
      else // the color is different from the previous one
      {
        if (!data->repetition_mode)                 // keep mode
        {
          if (data->list_size == 64)
          {
            if (PackBytes_pack_flush(data) < 0)
              return -1;
          }
          data->list[data->list_size++] = b;
        }
        else                                        // change mode
        {
          if (PackBytes_pack_flush(data) < 0)
            return -1;
          data->list[data->list_size++] = b;
        }
      }
  }
  return 0; // OK
}

int PackBytes_pack_flush(T_PackBytes_data * data)
{
  if (data->list_size > 0)
  {
    if (data->list_size > 64)
    {
      GFX2_Log(GFX2_ERROR, "PackBytes_pack_flush() list_size=%d !\n", data->list_size);
    }
    if (data->repetition_mode)
    {
      if (data->f != NULL)
      {
        if (!Write_byte(data->f, 0x40 + data->list_size - 1) ||
            !Write_byte(data->f, data->list[0]))
          return -1;
      }
      data->output_count += 2;
    }
    else
    {
      if (data->f != NULL)
      {
        if (!Write_byte(data->f, data->list_size - 1) ||
            !Write_bytes(data->f, data->list, data->list_size))
          return -1;
      }
      data->output_count += 1 + data->list_size;
    }
    data->list_size = 0;
    data->repetition_mode = 0;
  }
  return data->output_count;
}

int PackBytes_pack_buffer(FILE * f, const byte * buffer, size_t size)
{
  T_PackBytes_data pb_data;

  PackBytes_pack_init(&pb_data, f);
  while (size-- > 0)
  {
    if (PackBytes_pack_add(&pb_data, *buffer++))
      return -1;
  }
  return PackBytes_pack_flush(&pb_data);
}
