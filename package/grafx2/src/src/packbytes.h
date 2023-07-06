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

///@file packbytes.h
/// Packbytes is used in Apple Preferred Format

#ifndef PACKBYTES_H_INCLUDED
#define PACKBYTES_H_INCLUDED

/**
 * Data used by the PackBytes packer
 */
typedef struct {
  FILE * f;
  int output_count;
  byte list_size;
  byte repetition_mode;
  byte list[65];
} T_PackBytes_data;

/**
 * init before packing
 *
 * @param data storage for packbits data
 * @param f FILE output or NULL (for no output)
 */
void PackBytes_pack_init(T_PackBytes_data * data, FILE * f);

/**
 * Add a byte to the packbits stream
 * @return -1 for error, 0 if OK
 */
int PackBytes_pack_add(T_PackBytes_data * data, byte b);

/**
 * Flush the packed data to the file
 *
 * @return -1 for error, or the size of the packed stream so far
 */
int PackBytes_pack_flush(T_PackBytes_data * data);

/**
 * Pack a full buffer to FILE
 * @param f FILE output or NULL (for no output)
 * @param buffer input buffer
 * @param size byte size of input buffer
 * @return -1 for error, or the size of the packed stream so far
 */
int PackBytes_pack_buffer(FILE * f, const byte * buffer, size_t size);

#endif
