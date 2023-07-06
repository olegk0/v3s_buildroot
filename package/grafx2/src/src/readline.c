/* vim:expandtab:ts=2 sw=2:
 */
/*  Grafx2 - The Ultimate 256-color bitmap paint program

        Copyright owned by various GrafX2 authors, see COPYRIGHT.txt for
   details.

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

#define UTF8

/**
 * @file readline.c
 * Text input GUI widget
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "const.h"
#include "struct.h"
#include "global.h"
#include "misc.h"
#include "osdep.h"
#include "errors.h"
#include "const.h"
#include "screen.h"
#include "readline.h"
#include "windows.h"
#include "input.h"
#include "engine.h"
#include "unicode.h"
#include "keycodes.h"

#if defined(__ANDROID__)
#include <SDL_screenkeyboard.h>
#endif

#ifdef _MSC_VER
#include <stdio.h>
#define strdup _strdup
#if _MSC_VER < 1900
#define snprintf _snprintf
#endif
#endif

// Virtual keyboard is ON by default on these platforms:
#if defined(__GP2X__) || defined(__WIZ__) || defined(__CAANOO__) ||            \
  defined(GCWZERO) || defined(__SWITCH__)
#define VIRT_KEY_DEFAULT_ON 1
#else
#define VIRT_KEY_DEFAULT_ON 0
#endif

#define TEXT_COLOR MC_Black
#define BACKGROUND_COLOR MC_Light
#define CURSOR_COLOR MC_Black
#define CURSOR_BACKGROUND_COLOR MC_Dark

//! remove a character from a string
static void
Remove_character(char* str, int position)
{
  for (; str[position] != '\0'; position++)
    str[position] = str[position + 1];
}

//! remove a character from a unicode string
static void
Remove_character_unicode(word* str, int position)
{
  for (; str[position] != '\0'; position++)
    str[position] = str[position + 1];
}

//! Insert a character in string at position
static void
Insert_character(char* str, char letter, int position)
{
  char temp_char;

  for (; letter != '\0'; position++) {
    // On mémorise le caractère qui se trouve en "position"
    temp_char = str[position];
    // On splotch la lettre à insérer
    str[position] = letter;
    // On place le caractère mémorisé dans "letter" comme nouvelle lettre à
    // insérer
    letter = temp_char;
  }
  // On termine la chaine
  str[position] = '\0';
}

//! Insert a character in unicode string at position
static void
Insert_character_unicode(word* str, word c, int position)
{
  for (;;) {
    word temp = str[position];
    str[position++] = c;
    if (c == 0)
      return; // we just have written the NULL terminator
    c = temp;
  }
}

//! Insert a string at the start of another. Up to MAX characters only.
//! @return actual number of chars inserted
static int
Prepend_string(char* dest, const char* src, int max)
{
  // Insert src before dest
  int sized = strlen(dest);
  int sizes = strlen(src);

  if (sized + sizes >= max) {
    sizes = max - sized;
  }

  memmove(dest + sizes, dest, sized + 1);
  memcpy(dest, src, sizes);
  return sizes;
}

//! Insert a unicode string at the start of another. Up to MAX characters only.
//! @return actual number of chars inserted
static int
Prepend_string_unicode(word* dest, const word* src, int max)
{
  // Insert src before dest
  int sized = Unicode_strlen(dest);
  int sizes = Unicode_strlen(src);

  if (sized + sizes >= max) {
    sizes = max - sized;
  }

  memmove(dest + sizes, dest, 2 * (sized + 1));
  memcpy(dest, src, sizes * 2);
  return sizes;
}

//! Check validy of character depending on input_type
//! @returns 0 = Not allowed
//! @returns 1 = Allowed
//! @returns 2 = Allowed only once at start of string (for - sign in numbers)
static int
Valid_character(word c, enum INPUT_TYPE input_type)
{
  // On va regarder si l'utilisateur le droit de se servir de cette touche
  switch (input_type) {
    case INPUT_TYPE_STRING:
      if ((c >= ' ' && c <= 255) || c == '\n')
        return 1;
      if (c & 0xff00) // unicode
        return 1;
      break;
    case INPUT_TYPE_INTEGER:
      if ((c >= '0') && (c <= '9'))
        return 1;
      break;
    case INPUT_TYPE_DECIMAL:
      if ((c >= '0') && (c <= '9'))
        return 1;
      else if (c == '-')
        return 2;
      else if (c == '.')
        return 1;
      break;
    case INPUT_TYPE_FILENAME: {
      // On regarde si la touche est autorisée
      // Sous Linux: Seul le / est strictement interdit, mais beaucoup
      // d'autres poseront des problèmes au shell, alors on évite.
      // Sous Windows : c'est moins grave car le fopen() échouerait de toutes
      // façons. AmigaOS4: Pas de ':' car utilisé pour les volumes.
#if defined(WIN32)
      char forbidden_char[] = { '/', '|', '?', '*', '<', '>', ':', '\\' };
#elif defined(__amigaos4__) || defined(__AROS__) || defined(__amigaos__)
      char forbidden_char[] = { '/', '|', '?', '*', '<', '>', ':' };
#else
      char forbidden_char[] = { '/', '|', '?', '*', '<', '>' };
#endif
      int position;

      if (c < ' ')
        return 0;

      for (position = 0; position < (long)sizeof(forbidden_char); position++)
        if (c == forbidden_char[position])
          return 0;
      return 1;
    }
    case INPUT_TYPE_HEXA:
      if ((c >= '0') && (c <= '9'))
        return 1;
      else if ((c >= 'A') && (c <= 'F'))
        return 1;
      else if ((c >= 'a') && (c <= 'f'))
        return 1;
      break;
  } // End du "switch(input_type)"
  return 0;
}

//! remove invalid characters
static void
Cleanup_string(char* str, int input_type)
{
  int i, j = 0;

  for (i = 0; str[i] != '\0'; i++) {
    if (Valid_character((unsigned char)(str[i]), input_type)) {
      str[j] = str[i];
      j++;
    }
  }
  str[j] = '\0';
}

//! remove invalid characters
static void
Cleanup_string_unicode(word* str, int input_type)
{
  int i, j = 0;

  for (i = 0; str[i] != 0; i++) {
    if (Valid_character(str[i], input_type)) {
      str[j] = str[i];
      j++;
    }
  }
  str[j] = 0;
}

//! Prints the string and the cursor on screen
//! @param x_pos, y_pos position on screen
//! @param str the string to display
//! @param position the cursor position
static void
Display_whole_string(word x_pos, word y_pos, const char* str, byte position)
{
  char cursor[2];
  Print_general(x_pos, y_pos, str, TEXT_COLOR, BACKGROUND_COLOR);

  cursor[0] = str[position] ? str[position] : ' ';
  cursor[1] = '\0';
  Print_general(x_pos + (position << 3) * Menu_factor_X,
                y_pos,
                cursor,
                CURSOR_COLOR,
                CURSOR_BACKGROUND_COLOR);
}

//! Prints the unicode string and the cursor on screen
//! @param x_pos, y_pos position on screen
//! @param str_unicode the string to display
//! @param position the cursor position
static void
Display_whole_string_unicode(word x_pos,
                             word y_pos,
                             const word* str_unicode,
                             byte position)
{
  word cursor[2];
  Print_general_unicode(
    x_pos, y_pos, str_unicode, TEXT_COLOR, BACKGROUND_COLOR);

  cursor[0] = str_unicode[position] ? str_unicode[position] : ' ';
  cursor[1] = 0;
  Print_general_unicode(x_pos + (position << 3) * Menu_factor_X,
                        y_pos,
                        cursor,
                        CURSOR_COLOR,
                        CURSOR_BACKGROUND_COLOR);
}

//! Initializes and displays the visual keyboard
void
Init_virtual_keyboard(word y_pos, word keyboard_width, word keyboard_height)
{
  int h_pos;
  int v_pos;
  int parent_window_x = Window_pos_X + 2;

  h_pos = Window_pos_X + (keyboard_width - Window_width) * Menu_factor_X / -2;
  if (h_pos < 0)
    h_pos = 0;
  else if (h_pos + keyboard_width * Menu_factor_X > Screen_width)
    h_pos = Screen_width - keyboard_width * Menu_factor_X;
  v_pos = Window_pos_Y + (y_pos + 9) * Menu_factor_Y;
  if (v_pos + (keyboard_height * Menu_factor_Y) > Screen_height)
    v_pos = Window_pos_Y + (y_pos - keyboard_height - 4) * Menu_factor_Y;

  Hide_cursor();
  Open_popup(h_pos, v_pos, keyboard_width, keyboard_height);
  Window_rectangle(1, 0, Window_width - 1, Window_height - 1, MC_Light);
  Window_rectangle(0, 0, 1, Window_height - 2, MC_White);
  // white border on top left angle, when it exceeds border.
  if (parent_window_x > Window_pos_X)
    Window_rectangle(
      0, 0, (parent_window_x - Window_pos_X) / Menu_factor_X, 1, MC_White);
  Window_rectangle(2, Window_height - 2, Window_width - 2, 2, MC_Black);
  if (keyboard_width < 320) {
    Window_rectangle(Window_width - 2, 2, 2, Window_height - 2, MC_Black);
  }
}

/****************************************************************************
 *           Enhanced super scanf deluxe pro plus giga mieux :-)             *
 ****************************************************************************/
byte
Readline(word x_pos,
         word y_pos,
         char* str,
         byte visible_size,
         enum INPUT_TYPE input_type)
// Paramètres:
//   x_pos, y_pos : Coordonnées de la saisie dans la fenêtre
//   str       : Chaîne recevant la saisie (et contenant éventuellement une
//   valeur initiale) max_size  : Nombre de caractères logeant dans la zone de
//   saisie input_type  : 0=Chaîne, 1=Nombre, 2=Nom de fichier
// Sortie:
//   0: Sortie par annulation (Esc.) / 1: sortie par acceptation (Return)
{
  byte max_size;
  // Grosse astuce pour les noms de fichiers: La taille affichée est différente
  // de la taille maximum gérée.
  if (input_type == INPUT_TYPE_FILENAME)
    max_size = 255;
  else
    max_size = visible_size;
  return Readline_ex(x_pos, y_pos, str, visible_size, max_size, input_type, 0);
}

/****************************************************************************
 *           Enhanced super scanf deluxe pro plus giga mieux :-)             *
 ****************************************************************************/
byte
Readline_ex(word x_pos,
            word y_pos,
            char* str,
            byte visible_size,
            byte max_size,
            enum INPUT_TYPE input_type,
            byte decimal_places)
{
  return Readline_ex_unicode(x_pos,
                             y_pos,
                             str,
                             NULL,
                             visible_size,
                             max_size,
                             input_type,
                             decimal_places);
}

byte
Readline_ex_unicode(word x_pos,
                    word y_pos,
                    char* str,
                    word* str_unicode,
                    byte visible_size,
                    byte max_size,
                    enum INPUT_TYPE input_type,
                    byte decimal_places)
// Paramètres:
//   x_pos, y_pos : Coordonnées de la saisie dans la fenêtre
//   str       : Chaîne recevant la saisie (et contenant éventuellement une
//   valeur initiale) max_size  : Nombre de caractères logeant dans la zone de
//   saisie input_type  : 0=String, 1=Unsigned int, 2=Filename 3=Signed Double
//   decimal_places: Number of decimal places for a double
// Sortie:
//   0: Sortie par annulation (Esc.) / 1: sortie par acceptation (Return)
{
  char initial_string[256];
  char display_string[256];
  word initial_string_unicode[256];
  word display_string_unicode[256];
  byte position;
  size_t size;
  word input_char = 0;
  word input_key = 0;
  word window_x = Window_pos_X;
  word window_y = Window_pos_Y;
  byte offset = 0; // index du premier caractère affiché

  // Virtual keyboard
  byte use_virtual_keyboard = 0;
  static byte caps_lock = 0;
  static const word keymapping[] = {
    KEY_CLEAR,    KEY_BACKSPACE, KEY_RETURN, KEY_ESC, '0', '1', '2',
    '3',          '4',           '5',        '6',     '7', '8', '9',
    '.',          ',',           'Q',        'W',     'E', 'R', 'T',
    'Y',          'U',           'I',        'O',     'P', 'A', 'S',
    'D',          'F',           'G',        'H',     'J', 'K', 'L',
    KEY_CAPSLOCK, 'Z',           'X',        'C',     'V', 'B', 'N',
    'M',          ' ',           '-',        '+',     '*', '/', '|',
    '\\',         '(',           ')',        '{',     '}', '[', ']',
    '_',          '=',           '<',        '>',     '%', '@', ':',
    ';',          '`',           '\'',       '"',     '~', '!', '?',
    '^',          '&',           '#',        '$'
  };

  // Si on a commencé à editer par un clic-droit, on vide la chaine.
  if (Mouse_K == RIGHT_SIDE) {
    str[0] = '\0';
    position = 0;
  } else {
    int int_pos = ((Mouse_X - Window_pos_X) / Menu_factor_X - x_pos) >> 3;
    if (Mouse_K == LEFT_SIDE)
      position = (int_pos >= 0 && int_pos <= 255) ? (byte)int_pos : 255;
    else
      position = 255; // always edit from the tail if using hotkey

    if (input_type == INPUT_TYPE_INTEGER && str[0] != '\0')
      snprintf(str, 10, "%d", atoi(str)); // align left
    else if (input_type == INPUT_TYPE_DECIMAL) {
      //  Nothing. The caller should have used Sprint_double, with min_positions
      //  at zero, so there's no spaces on the left and no useless 0s on the
      //  right.
    } else if (input_type == INPUT_TYPE_HEXA) {
      //  Nothing. The caller should have initialized a valid hexa number.
    }
  }

#if defined(__ANDROID__)
  SDL_ANDROID_GetScreenKeyboardTextInput(str, max_size);
  input_key = KEY_RETURN;
#else
  // Virtual keyboards
  if (Config.Use_virtual_keyboard == 1 ||
      (VIRT_KEY_DEFAULT_ON && Config.Use_virtual_keyboard == 0)) {
    if (input_type == INPUT_TYPE_STRING || input_type == INPUT_TYPE_FILENAME) {
      int x, y;

      Init_virtual_keyboard(y_pos, 320, 87);

      use_virtual_keyboard = 1;

      // The order is important, see the array

      Window_set_normal_button(7, 67, 43, 15, "Clr", 0, 1, KEY_NONE);
      Window_set_normal_button(51, 67, 43, 15, "Del", 0, 1, KEY_NONE);
      Window_set_normal_button(95, 67, 43, 15, "OK", 0, 1, KEY_NONE);
      Window_set_normal_button(139, 67, 43, 15, "Esc", 0, 1, KEY_NONE);
      Window_display_frame_in(5, 65, 179, 19);

      Window_set_normal_button(193, 63, 17, 19, "0", 0, 1, KEY_NONE);
      Window_set_normal_button(193, 43, 17, 19, "1", 0, 1, KEY_NONE);
      Window_set_normal_button(211, 43, 17, 19, "2", 0, 1, KEY_NONE);
      Window_set_normal_button(229, 43, 17, 19, "3", 0, 1, KEY_NONE);
      Window_set_normal_button(193, 23, 17, 19, "4", 0, 1, KEY_NONE);
      Window_set_normal_button(211, 23, 17, 19, "5", 0, 1, KEY_NONE);
      Window_set_normal_button(229, 23, 17, 19, "6", 0, 1, KEY_NONE);
      Window_set_normal_button(193, 3, 17, 19, "7", 0, 1, KEY_NONE);
      Window_set_normal_button(211, 3, 17, 19, "8", 0, 1, KEY_NONE);
      Window_set_normal_button(229, 3, 17, 19, "9", 0, 1, KEY_NONE);
      Window_set_normal_button(211, 63, 17, 19, ".", 0, 1, KEY_NONE);
      Window_set_normal_button(229, 63, 17, 19, ",", 0, 1, KEY_NONE);

      Window_set_normal_button(3, 3, 18, 19, "Q", 0, 1, KEY_NONE);
      Window_set_normal_button(22, 3, 18, 19, "W", 0, 1, KEY_NONE);
      Window_set_normal_button(41, 3, 18, 19, "E", 0, 1, KEY_NONE);
      Window_set_normal_button(60, 3, 18, 19, "R", 0, 1, KEY_NONE);
      Window_set_normal_button(79, 3, 18, 19, "T", 0, 1, KEY_NONE);
      Window_set_normal_button(98, 3, 18, 19, "Y", 0, 1, KEY_NONE);
      Window_set_normal_button(117, 3, 18, 19, "U", 0, 1, KEY_NONE);
      Window_set_normal_button(136, 3, 18, 19, "I", 0, 1, KEY_NONE);
      Window_set_normal_button(155, 3, 18, 19, "O", 0, 1, KEY_NONE);
      Window_set_normal_button(174, 3, 18, 19, "P", 0, 1, KEY_NONE);

      Window_set_normal_button(12, 23, 18, 19, "A", 0, 1, KEY_NONE);
      Window_set_normal_button(31, 23, 18, 19, "S", 0, 1, KEY_NONE);
      Window_set_normal_button(50, 23, 18, 19, "D", 0, 1, KEY_NONE);
      Window_set_normal_button(69, 23, 18, 19, "F", 0, 1, KEY_NONE);
      Window_set_normal_button(88, 23, 18, 19, "G", 0, 1, KEY_NONE);
      Window_set_normal_button(107, 23, 18, 19, "H", 0, 1, KEY_NONE);
      Window_set_normal_button(126, 23, 18, 19, "J", 0, 1, KEY_NONE);
      Window_set_normal_button(145, 23, 18, 19, "K", 0, 1, KEY_NONE);
      Window_set_normal_button(164, 23, 18, 19, "L", 0, 1, KEY_NONE);

      Window_set_normal_button(
        3, 43, 18, 19, caps_lock ? "\036" : "\037", 0, 1, KEY_NONE);
      Window_set_normal_button(22, 43, 18, 19, "Z", 0, 1, KEY_NONE);
      Window_set_normal_button(41, 43, 18, 19, "X", 0, 1, KEY_NONE);
      Window_set_normal_button(60, 43, 18, 19, "C", 0, 1, KEY_NONE);
      Window_set_normal_button(79, 43, 18, 19, "V", 0, 1, KEY_NONE);
      Window_set_normal_button(98, 43, 18, 19, "B", 0, 1, KEY_NONE);
      Window_set_normal_button(117, 43, 18, 19, "N", 0, 1, KEY_NONE);
      Window_set_normal_button(136, 43, 18, 19, "M", 0, 1, KEY_NONE);
      Window_set_normal_button(155, 43, 18, 19, " ", 0, 1, KEY_NONE);

      for (y = 0; y < 5; y++) {
        for (x = 0; x < 6; x++) {
          char label[2] = " ";
          label[0] = keymapping[x + y * 6 + 44];
          Window_set_normal_button(
            247 + x * 12, 3 + y * 16, 11, 15, label, 0, 1, KEY_NONE);
        }
      }

      Update_window_area(0, 0, Window_width, Window_height);
      Display_cursor();
    } else if (input_type == INPUT_TYPE_INTEGER ||
               input_type == INPUT_TYPE_DECIMAL) {
      Init_virtual_keyboard(y_pos, 215, 47);

      use_virtual_keyboard = 1;

      // The order is important, see the array

      Window_set_normal_button(7, 27, 43, 15, "Clr", 0, 1, KEY_NONE);
      Window_set_normal_button(51, 27, 43, 15, "Del", 0, 1, KEY_NONE);
      Window_set_normal_button(95, 27, 43, 15, "OK", 0, 1, KEY_NONE);
      Window_set_normal_button(139, 27, 43, 15, "Esc", 0, 1, KEY_NONE);
      Window_display_frame_in(5, 25, 179, 19);

      Window_set_normal_button(174, 3, 18, 19, "0", 0, 1, KEY_NONE);
      Window_set_normal_button(3, 3, 18, 19, "1", 0, 1, KEY_NONE);
      Window_set_normal_button(22, 3, 18, 19, "2", 0, 1, KEY_NONE);
      Window_set_normal_button(41, 3, 18, 19, "3", 0, 1, KEY_NONE);
      Window_set_normal_button(60, 3, 18, 19, "4", 0, 1, KEY_NONE);
      Window_set_normal_button(79, 3, 18, 19, "5", 0, 1, KEY_NONE);
      Window_set_normal_button(98, 3, 18, 19, "6", 0, 1, KEY_NONE);
      Window_set_normal_button(117, 3, 18, 19, "7", 0, 1, KEY_NONE);
      Window_set_normal_button(136, 3, 18, 19, "8", 0, 1, KEY_NONE);
      Window_set_normal_button(155, 3, 18, 19, "9", 0, 1, KEY_NONE);
      Window_set_normal_button(193, 3, 18, 19, ".", 0, 1, KEY_NONE);

      Update_window_area(0, 0, Window_width, Window_height);
      Display_cursor();
    }
  }
#ifdef GCWZERO // we cannot enter text into a field without using the virtual
               // mouse otherwise so no saving etc
  Keyboard_click_allowed = 1;
#else
  Keyboard_click_allowed = 0;
#endif
  Hide_cursor();

  // Mise à jour des variables se rapportant à la chaîne en fonction de la
  // chaîne initiale
  strcpy(initial_string, str);
  if (str_unicode != NULL) {
    int padding;

    size = Unicode_strlen(str_unicode);
    if (size == 0) {
      size = Str_to_utf8(str_unicode, str, max_size);
    } else {
      if (size > 255)
        size = 255;
    }
    memcpy(initial_string_unicode, str_unicode, 2 * (size + 1));
    if (position >= size)
      position = (byte)((size <= max_size) ? size : size - 1);
    if (position - offset >= visible_size)
      offset = position - visible_size + 1;
    // copy only part of the string if it is too long
    Unicode_strlcpy(display_string_unicode, str_unicode + offset, visible_size);
    if (offset > 0)
      display_string_unicode[0] = (byte)LEFT_TRIANGLE_CHARACTER;
    if ((size_t)visible_size + offset + 1 < size)
      display_string_unicode[visible_size - 1] = (byte)RIGHT_TRIANGLE_CHARACTER;
    // pad the right side of visible area with spaces to erase any garbage
    padding = visible_size - Unicode_strlen(display_string_unicode);
    while (padding > 0) {
      Unicode_char_strlcat(display_string_unicode, " ", 1);
      padding--;
    }

    Display_whole_string_unicode(window_x + (x_pos * Menu_factor_X),
                                 window_y + (y_pos * Menu_factor_Y),
                                 str_unicode,
                                 position - offset);
  } else {
    int padding;
    printf("str\n");
    size = strlen(str);
    if (size > 255)
      size = 255;
    if (position >= size)
      position = (byte)((size <= max_size) ? size : size - 1);
    if (position - offset >= visible_size)
      offset = position - visible_size + 1;
    // copy only part of the string if it is too long
    strncpy(display_string, str + offset, visible_size);
    display_string[visible_size] = '\0';
    if (offset > 0)
      display_string[0] = LEFT_TRIANGLE_CHARACTER;
    if ((size_t)visible_size + offset + 1 < size)
      display_string[visible_size - 1] = RIGHT_TRIANGLE_CHARACTER;
    // pad the right side of visible area with spaces to erase any garbage
    padding = visible_size - strlen(display_string);
    while (padding > 0) {
      strcat(display_string, " ");
      padding--;
    }

    Display_whole_string(window_x + (x_pos * Menu_factor_X),
                         window_y + (y_pos * Menu_factor_Y),
                         display_string,
                         position - offset);
  }
  Update_rect(window_x + (x_pos * Menu_factor_X),
              window_y + (y_pos * Menu_factor_Y),
              visible_size * (Menu_factor_X << 3),
              (Menu_factor_Y << 3));
  Flush_update();
  if (Mouse_K) {
    Display_cursor();
    Wait_end_of_click();
    Hide_cursor();
  } else {
    // discard hotkey used to activate the button
    Get_input(0);
  }

  while ((input_key != KEY_RETURN) && (input_key != KEY_ESC)) {
    Display_cursor();
    if (use_virtual_keyboard) {
      int clicked_button;

      clicked_button = Window_clicked_button();
      input_char = 0;
      input_key = Key;

      if (clicked_button == -1)
        input_key = KEY_RETURN;
      else if (clicked_button > 0) {
        input_key = keymapping[clicked_button - 1];
        if (input_key == KEY_CAPSLOCK) {
          // toggle uppercase
          caps_lock = !caps_lock;
          Hide_cursor();
          Print_in_window(
            8, 49, caps_lock ? "\036" : "\037", MC_Black, MC_Light);
          Display_cursor();
        } else if (input_key == KEY_BACKSPACE) {
          // A little hack: the button for backspace will:
          // - backspace if the cursor is at end of string
          // - delete otherwise
          // It's needed for those input boxes that are completely full.
          if (position < size)
            input_key = KEY_DELETE;
        } else if (input_key >= 'A' && input_key <= 'Z' && !caps_lock) {
          input_char = input_key + 'a' - 'A';
        } else if (input_key >= ' ') // this is a printable char
        {
          input_char = input_key;
        }
      }
    } else {
      do {
        Get_input(20);
        input_char = (str_unicode == NULL) ? Key_ANSI : Key_UNICODE;
        input_key = Key;
        if (Mouse_K)
          input_key = KEY_RETURN;

        // Handle paste request on CTRL+v
        if (input_key == SHORTCUT_PASTE) {
          int nb_added = 0;
          if (str_unicode != NULL) {
            word* data_unicode = NULL;
            char* data = GFX2_GetTextClipboard(&data_unicode);
            if (data_unicode != NULL) {
              // ignore ANSI data, use Unicode
              Cleanup_string_unicode(data_unicode, input_type);
              nb_added = Prepend_string_unicode(
                str_unicode + position, data_unicode, max_size - position);
              free(data_unicode);
            } else {
              word tmp_unicode[256];
              if (data == NULL)
                continue;
              Cleanup_string(data, input_type);
              Unicode_char_strlcpy(tmp_unicode,
                                   data,
                                   sizeof(tmp_unicode) /
                                     sizeof(word)); // convert ANSI to unicode
              nb_added = Prepend_string_unicode(
                str_unicode + position, tmp_unicode, max_size - position);
            }
            free(data);
          } else {
            char* data = GFX2_GetTextClipboard(NULL);
            if (data == NULL)
              continue; // No clipboard data
            Cleanup_string(data, input_type);
            // Insert it at the cursor position
            nb_added =
              Prepend_string(str + position, data, max_size - position);
            free(data);
          }
          while (nb_added > 0) {
            size++;
            if (size < max_size) {
              position++;
              if (str_unicode != NULL) {
                if (display_string_unicode[position - offset] ==
                      (byte)RIGHT_TRIANGLE_CHARACTER ||
                    position - offset >= visible_size)
                  offset++;
              } else {
                if (display_string[position - offset] ==
                      RIGHT_TRIANGLE_CHARACTER ||
                    position - offset >= visible_size)
                  offset++;
              }
            }
            nb_added--;
          }
          Hide_cursor();
          goto affichage;
        }

      } while (input_char == 0 && input_key == 0
#if defined(USE_SDL2)
               && Key_Text[0] == '\0'
#endif
      );
    }
    Hide_cursor();

#if defined(USE_SDL2)
    if (Key_Text[0] != 0) {
      word unicode_text[32];
      int i;
#if defined(ENABLE_FILENAMES_ICONV)
      char* input = Key_Text;
      size_t inbytesleft = strlen(Key_Text);
      char* output = (char*)unicode_text;
      size_t outbytesleft = sizeof(unicode_text) - 2;
      size_t r = iconv(cd_utf16, &input, &inbytesleft, &output, &outbytesleft);
      if (r != (size_t)-1) {
        output[1] = output[0] = '\0';
      } else {
        GFX2_Log(GFX2_WARNING, "Unicode conversion of input text failed\n");
        unicode_text[0] = 0;
      }
#elif defined(WIN32)
      Unicode_from_utf8(
        Key_Text, unicode_text, sizeof(unicode_text) / sizeof(word));
#else
      int j;
      for (i = 0, j = 0; i < (int)sizeof(Key_Text) &&
                         j < (int)sizeof(unicode_text) && Key_Text[i] != '\0';
           i++) {
        // Only ASCII chars
        if ((Key_Text[i] & ~127) == 0)
          unicode_text[j++] = Key_Text[i];
      }
      unicode_text[j] = 0;
      if (i != j)
        GFX2_Log(GFX2_INFO,
                 "Key_Text[] not fully converted to unicode Key_Text='%s'\n",
                 Key_Text);
#endif
      for (i = 0; unicode_text[i] != 0 && size < max_size; i++) {
        // Si la touche était autorisée...
        byte is_authorized = Valid_character(unicode_text[i], input_type);
        if (is_authorized == 1 ||
            (is_authorized == 2 && position == 0 && str[position] != '-')) {
          // ... alors on l'insère ...
          if (str_unicode != NULL)
            Insert_character_unicode(
              str_unicode, unicode_text[i], position /*,size*/);
          else
            Insert_character(str, unicode_text[i], position /*,size*/);
          // ce qui augmente la taille de la chaine
          size++;
          // et qui risque de déplacer le curseur vers la droite
          if (size <= max_size) {
            position++;
            if (position - offset >= visible_size)
              offset++;
            else if (str_unicode != NULL) {
              if (display_string_unicode[position - offset] ==
                  RIGHT_TRIANGLE_CHARACTER)
                offset++;
            } else {
              if (display_string[position - offset] == RIGHT_TRIANGLE_CHARACTER)
                offset++;
            }
          }
        }
      }
      // Enfin, on raffiche la chaine
      goto affichage;
    } else
#endif
      switch (input_key) {
        case KEY_DELETE: // Suppr.
          if (position < size) {
            if (str_unicode != NULL)
              Remove_character_unicode(str_unicode, position);
            else
              Remove_character(str, position);
            size--;

            // Effacement de la chaîne
            Screen_FillRect((window_x + (x_pos * Menu_factor_X)) * Pixel_width,
                            (window_y + (y_pos * Menu_factor_Y)) * Pixel_height,
                            (visible_size * (Menu_factor_X << 3)) * Pixel_width,
                            (Menu_factor_Y << 3) * Pixel_height,
                            BACKGROUND_COLOR);
            goto affichage;
          }
          break;
        case KEY_LEFT: // Gauche
          if (position > 0) {
            char at_end_of_string;
            if (str_unicode != NULL)
              at_end_of_string = (str_unicode[position] == '\0');
            else
              at_end_of_string = (str[position] == '\0');
            // Effacement de la chaîne
            if (position == size)
              Screen_FillRect(
                (window_x + (x_pos * Menu_factor_X)) * Pixel_width,
                (window_y + (y_pos * Menu_factor_Y)) * Pixel_height,
                (visible_size * (Menu_factor_X << 3)) * Pixel_width,
                (Menu_factor_Y << 3) * Pixel_height,
                BACKGROUND_COLOR);
            position--;
            if (offset > 0 && (position < (offset + 1) || at_end_of_string))
              offset--;
            goto affichage;
          }
          break;
        case KEY_RIGHT: // Droite
          if ((position < size) && (position < max_size)) {
            position++;
            if (str_unicode != NULL) {
              if (display_string_unicode[position - offset] ==
                    (byte)RIGHT_TRIANGLE_CHARACTER ||
                  position - offset >= visible_size)
                offset++;
            } else {
              if (display_string[position - offset] ==
                    RIGHT_TRIANGLE_CHARACTER ||
                  position - offset >= visible_size)
                offset++;
            }
            goto affichage;
          }
          break;
        case KEY_HOME: // Home
          if (position) {
            // Effacement de la chaîne
            if (position == size)
              Screen_FillRect(
                (window_x + (x_pos * Menu_factor_X)) * Pixel_width,
                (window_y + (y_pos * Menu_factor_Y)) * Pixel_height,
                (visible_size * (Menu_factor_X << 3)) * Pixel_width,
                (Menu_factor_Y << 3) * Pixel_height,
                BACKGROUND_COLOR);
            position = 0;
            offset = 0;
            goto affichage;
          }
          break;
        case KEY_END: // End
          if ((position < size) && (position < max_size - 1)) {
            position = (byte)((size < max_size) ? size : size - 1);
            if (position - offset >= visible_size)
              offset = position - visible_size + 1;
            goto affichage;
          }
          break;
        case KEY_BACKSPACE: // Backspace : combinaison de gauche + suppr

          if (position > 0) {
            char at_end_of_string;
            if (str_unicode != NULL)
              at_end_of_string = (str_unicode[position] == '\0');
            else
              at_end_of_string = (str[position] == '\0');
            position--;
            if (offset > 0 && ((position < (offset + 2)) || at_end_of_string ||
                               !(display_string[visible_size - 1] ==
                                 RIGHT_TRIANGLE_CHARACTER)))
              offset--;
            if (str_unicode != NULL)
              Remove_character_unicode(str_unicode, position);
            else
              Remove_character(str, position);
            size--;
            // Effacement de la chaîne
            Screen_FillRect((window_x + (x_pos * Menu_factor_X)) * Pixel_width,
                            (window_y + (y_pos * Menu_factor_Y)) * Pixel_height,
                            (visible_size * (Menu_factor_X << 3)) * Pixel_width,
                            (Menu_factor_Y << 3) * Pixel_height,
                            BACKGROUND_COLOR);
            goto affichage;
          }
          break;
        case KEY_CLEAR: // Clear
          str[0] = '\0';
          if (str_unicode != NULL)
            str_unicode[0] = 0;
          position = offset = 0;
          // Effacement de la chaîne
          Screen_FillRect((window_x + (x_pos * Menu_factor_X)) * Pixel_width,
                          (window_y + (y_pos * Menu_factor_Y)) * Pixel_height,
                          (visible_size * (Menu_factor_X << 3)) * Pixel_width,
                          (Menu_factor_Y << 3) * Pixel_height,
                          BACKGROUND_COLOR);
          goto affichage;
        case KEY_RETURN:
          break;

        case KEY_ESC:
          // On restaure la chaine initiale
          strcpy(str, initial_string);
          size = strlen(str);
          if (size > 255)
            size = 255;
          if (str_unicode != NULL) {
            Unicode_strlcpy(str_unicode, initial_string_unicode, 256);
            size = Unicode_strlen(str_unicode);
          }
          break;
        default:
          // SDL2 keystrokes are reported through both Key_Text and
          // Key_ANSI / Key_UNICODE
#if defined(USE_SDL2)
          if (use_virtual_keyboard && size < max_size && input_char != 0)
#else
        if (size < max_size && input_char != 0)
#endif
          {

            // Si la touche était autorisée...
            byte is_authorized = Valid_character(input_char, input_type);
            if (is_authorized == 1 ||
                (is_authorized == 2 && position == 0 && str[position] != '-')) {
              // ... alors on l'insère ...
              if (str_unicode != NULL)
                Insert_character_unicode(
                  str_unicode, input_char, position /*,size*/);
              else
                Insert_character(str, input_char, position /*,size*/);
              // ce qui augmente la taille de la chaine
              size++;
              // et qui risque de déplacer le curseur vers la droite
              if (size < max_size) {
                position++;
                if (position - offset >= visible_size)
                  offset++;
                else if (str_unicode != NULL) {
                  if (display_string_unicode[position - offset] ==
                      RIGHT_TRIANGLE_CHARACTER)
                    offset++;
                } else {
                  if (display_string[position - offset] ==
                      RIGHT_TRIANGLE_CHARACTER)
                    offset++;
                }
              }
              // Enfin, on raffiche la chaine
              goto affichage;
            } // End du test d'autorisation de touche
          }   // End du test de place libre
          break;

        affichage:
          if (str_unicode != NULL) {
            size = Unicode_strlen(str_unicode);
            if (size > 255)
              size = 255;
            // only show part of the string if too long
            Unicode_strlcpy(
              display_string_unicode, str_unicode + offset, visible_size);
            if (offset > 0)
              display_string_unicode[0] = (byte)LEFT_TRIANGLE_CHARACTER;
            if ((size_t)visible_size + offset + 0 < size)
              display_string_unicode[visible_size - 1] =
                (byte)RIGHT_TRIANGLE_CHARACTER;
            Display_whole_string_unicode(window_x + (x_pos * Menu_factor_X),
                                         window_y + (y_pos * Menu_factor_Y),
                                         display_string_unicode,
                                         position - offset);
          } else {
            size = strlen(str);
            if (size > 255)
              size = 255;
            // only show part of the string if too long
            strncpy(display_string, str + offset, visible_size);
            display_string[visible_size] = '\0';
            if (offset > 0)
              display_string[0] = LEFT_TRIANGLE_CHARACTER;
            if ((size_t)visible_size + offset + 0 < size)
              display_string[visible_size - 1] = RIGHT_TRIANGLE_CHARACTER;
            Display_whole_string(window_x + (x_pos * Menu_factor_X),
                                 window_y + (y_pos * Menu_factor_Y),
                                 display_string,
                                 position - offset);
          }
          Update_rect(window_x + (x_pos * Menu_factor_X),
                      window_y + (y_pos * Menu_factor_Y),
                      visible_size * (Menu_factor_X << 3),
                      (Menu_factor_Y << 3));
      } // End du "switch(input_key)"
    Flush_update();

  } // End du "while"
  Keyboard_click_allowed = 1;
  if (use_virtual_keyboard) {
    byte old_mouse_k = Mouse_K;
    Close_popup();
    Mouse_K = old_mouse_k;
    Input_sticky_control = 0;
  }
#endif // defined(__ANDROID__)
  // Effacement de la chaîne
  Screen_FillRect((window_x + (x_pos * Menu_factor_X)) * Pixel_width,
                  (window_y + (y_pos * Menu_factor_Y)) * Pixel_height,
                  (visible_size * (Menu_factor_X << 3)) * Pixel_width,
                  (Menu_factor_Y << 3) * Pixel_height,
                  BACKGROUND_COLOR);

  // On raffiche la chaine correctement
  if (input_type == INPUT_TYPE_INTEGER) {
    if (str[0] == '\0') {
      strcpy(str, "0");
      size = 1;
    }
    Print_in_window(x_pos + (((short)max_size - (short)size) << 3),
                    y_pos,
                    str,
                    TEXT_COLOR,
                    BACKGROUND_COLOR);
  } else if (input_type == INPUT_TYPE_DECIMAL) {
    double value;
    // Discard extra digits
    value = Fround(atof(str), decimal_places);
    Sprint_double(str, value, decimal_places, visible_size);
    // Recompute updated size
    size = strlen(str);
    if (size > 255)
      size = 255;

    if (size <= visible_size)
      Print_in_window(x_pos + (((short)visible_size - (short)size) << 3),
                      y_pos,
                      str,
                      TEXT_COLOR,
                      BACKGROUND_COLOR);
    else
      Print_in_window_limited(
        x_pos, y_pos, str, visible_size, TEXT_COLOR, BACKGROUND_COLOR);
  } else {
    if (str_unicode != NULL) {
#ifdef UTF8
      size = Utf8_to_str(str, str_unicode, max_size);
      // printf("s:%s  l:%ld\n", str, size);
#endif
      Print_in_window_limited_unicode(
        x_pos, y_pos, str_unicode, visible_size, TEXT_COLOR, BACKGROUND_COLOR);
    } else {
      Print_in_window_limited(
        x_pos, y_pos, str, visible_size, TEXT_COLOR, BACKGROUND_COLOR);
    }
  }
  Update_window_area(x_pos, y_pos, visible_size << 3, 8);

  return (input_key == KEY_RETURN);
}

void
Sprint_double(char* str, double value, byte decimal_places, byte min_positions)
{
  int i;
  int length;

  sprintf(str, "%.*f", decimal_places, value);
  length = strlen(str);

  for (i = 0; i < length; i++) {
    if (str[i] == '.') {
      // Remove extraneous zeroes
      char* decimals = str + i + 1;
      int j;

      for (j = strlen(decimals) - 1; j >= 0 && decimals[j] == '0'; j--) {
        decimals[j] = '\0';
      }
      // If all decimals were removed, remove the dot too
      if (str[i + 1] == '\0')
        str[i] = '\0';

      // Update string length
      length = strlen(str);

      // Ends the parent loop
      break;
    }
  }

  // Now try add spaces at beginning
  if (length < min_positions) {
    int offset = min_positions - length;

    // Move the string to the right
    for (i = 0; i <= length; i++) {
      str[length + offset - i] = str[length - i];
    }
    // Replace the N first characters by spaces
    for (i = 0; i < offset; i++) {
      str[i] = ' ';
    }
  }
}
