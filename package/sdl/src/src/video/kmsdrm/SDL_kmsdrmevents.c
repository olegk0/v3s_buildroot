// SPDX-License-Identifier: LGPL-2.1-or-later
/*
 * Copyright (C) 2020 Paul Cercueil <paul@crapouillou.net>
 *
 * libudev/libevdev input code for SDL1
 */

#ifdef UDEV

#include "SDL_config.h"
#include "SDL_keysym.h"

#include "SDL.h"
#include "../../events/SDL_sysevents.h"
#include "../../events/SDL_events_c.h"

#include "SDL_kmsdrmvideo.h"
#include "SDL_kmsdrmevents_c.h"

#include <errno.h>
#include <libudev.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <unistd.h>

static drm_input_dev*
KMSDRM_GetInputDevice(const char* property)
{
  drm_input_dev *new_dev, *input_devs = NULL;
  struct udev_enumerate* enumerate;
  struct udev_list_entry *devices, *entry;
  struct udev_device* dev;
  struct udev* udev;
  const char* path;

  udev = udev_new();
  if (!udev) {
    SDL_SetError("Could not create libudev instance.\n");
    return (NULL);
  }

  enumerate = udev_enumerate_new(udev);
  if (!enumerate) {
    SDL_SetError("Could not create libudev enumerate instance.\n");
    goto out_unref;
  }

  udev_enumerate_add_match_subsystem(enumerate, "input");
  udev_enumerate_add_match_property(enumerate, property, "1");
  udev_enumerate_scan_devices(enumerate);

  devices = udev_enumerate_get_list_entry(enumerate);
  if (!devices) {
    SDL_SetError("Failed to get device list.\n");
    goto out_unref_enumerate;
  }

  udev_list_entry_foreach(entry, devices)
  {
    dev = udev_device_new_from_syspath(udev, udev_list_entry_get_name(entry));
    if (!dev) {
      SDL_SetError("Failed to create device.\n");
      goto out_unref_enumerate;
    }

    path = udev_device_get_devnode(dev);
    if (!path) {
      udev_device_unref(dev);
      continue;
    }

    new_dev = SDL_malloc(sizeof(*new_dev));
    if (!new_dev) {
      udev_device_unref(dev);
      goto out_unref_enumerate;
    }

    new_dev->fd = open(path, O_RDONLY | O_NONBLOCK | O_CLOEXEC);
    if (new_dev->fd < 0) {
      SDL_SetError("Could not open device '%s'\n", path);
      SDL_free(new_dev);
      udev_device_unref(dev);
      goto out_unref_enumerate;
    }

    new_dev->path = SDL_strdup(path);
    if (!new_dev->path) {
      close(new_dev->fd);
      SDL_free(new_dev);
      udev_device_unref(dev);
      goto out_unref_enumerate;
    }

    new_dev->next = input_devs;
    input_devs = new_dev;

    udev_device_unref(dev);
  }

out_unref_enumerate:
  udev_enumerate_unref(enumerate);
out_unref:
  udev_unref(udev);
  return (input_devs);
}

int
KMSDRM_InitInput(_THIS)
{
  drm_input_dev* devs;

  devs = KMSDRM_GetInputDevice("ID_INPUT_KEY");
  this->hidden->keyboards = devs;

  for (; devs; devs = devs->next)
    kmsdrm_dbg_printf("Found keyboard: %s\n", devs->path);

  devs = KMSDRM_GetInputDevice("ID_INPUT_MOUSE");
  this->hidden->mice = devs;

  for (; devs; devs = devs->next)
    kmsdrm_dbg_printf("Found mouse: %s\n", devs->path);

  return 0;
}

void
KMSDRM_ExitInput(_THIS)
{
  drm_input_dev *devs, *next;

  for (devs = this->hidden->keyboards; devs; devs = next) {
    next = devs->next;

    SDL_free(devs->path);
    close(devs->fd);
    SDL_free(devs);
  }
}

static const SDLKey keymap[] = {
  [KEY_ESC] = SDLK_ESCAPE,
  [KEY_1] = SDLK_1,
  [KEY_2] = SDLK_2,
  [KEY_3] = SDLK_3,
  [KEY_4] = SDLK_4,
  [KEY_5] = SDLK_5,
  [KEY_6] = SDLK_6,
  [KEY_7] = SDLK_7,
  [KEY_8] = SDLK_8,
  [KEY_9] = SDLK_9,
  [KEY_0] = SDLK_0,
  [KEY_MINUS] = SDLK_MINUS,

  [KEY_EQUAL] = SDLK_EQUALS,
  [KEY_BACKSPACE] = SDLK_BACKSPACE,
  [KEY_TAB] = SDLK_TAB,
  [KEY_Q] = SDLK_q,
  [KEY_W] = SDLK_w,
  [KEY_E] = SDLK_e,
  [KEY_R] = SDLK_r,
  [KEY_T] = SDLK_t,
  [KEY_Y] = SDLK_y,
  [KEY_U] = SDLK_u,
  [KEY_I] = SDLK_i,
  [KEY_O] = SDLK_o,
  [KEY_P] = SDLK_p,
  [KEY_LEFTBRACE] = SDLK_LEFTBRACKET,
  [KEY_RIGHTBRACE] = SDLK_RIGHTBRACKET,
  [KEY_ENTER] = SDLK_RETURN,
  [KEY_LEFTCTRL] = SDLK_LCTRL,
  [KEY_A] = SDLK_a,
  [KEY_S] = SDLK_s,
  [KEY_D] = SDLK_d,
  [KEY_F] = SDLK_f,
  [KEY_G] = SDLK_g,
  [KEY_H] = SDLK_h,
  [KEY_J] = SDLK_j,
  [KEY_K] = SDLK_k,
  [KEY_L] = SDLK_l,
  [KEY_SEMICOLON] = SDLK_SEMICOLON,
  [KEY_APOSTROPHE] = SDLK_QUOTE,
  /* [KEY_GRAVE] = SDLK_GRAVE, ??? */
  [KEY_LEFTSHIFT] = SDLK_LSHIFT,
  [KEY_BACKSLASH] = SDLK_BACKSLASH,
  [KEY_Z] = SDLK_z,
  [KEY_X] = SDLK_x,
  [KEY_C] = SDLK_c,
  [KEY_V] = SDLK_v,
  [KEY_B] = SDLK_b,
  [KEY_N] = SDLK_n,
  [KEY_M] = SDLK_m,
  [KEY_COMMA] = SDLK_COMMA,
  [KEY_DOT] = SDLK_PERIOD,
  [KEY_SLASH] = SDLK_SLASH,
  [KEY_RIGHTSHIFT] = SDLK_RSHIFT,
  [KEY_KPASTERISK] = SDLK_KP_MULTIPLY,
  [KEY_LEFTALT] = SDLK_LALT,
  [KEY_SPACE] = SDLK_SPACE,
  [KEY_CAPSLOCK] = SDLK_CAPSLOCK,
  [KEY_F1] = SDLK_F1,
  [KEY_F2] = SDLK_F2,
  [KEY_F3] = SDLK_F3,
  [KEY_F4] = SDLK_F4,
  [KEY_F5] = SDLK_F5,
  [KEY_F6] = SDLK_F6,
  [KEY_F7] = SDLK_F7,
  [KEY_F8] = SDLK_F8,
  [KEY_F9] = SDLK_F9,
  [KEY_F10] = SDLK_F10,
  [KEY_NUMLOCK] = SDLK_NUMLOCK,
  [KEY_SCROLLLOCK] = SDLK_SCROLLOCK,
  [KEY_KP7] = SDLK_KP7,
  [KEY_KP8] = SDLK_KP8,
  [KEY_KP9] = SDLK_KP9,
  [KEY_KPMINUS] = SDLK_KP_MINUS,
  [KEY_KP4] = SDLK_KP4,
  [KEY_KP5] = SDLK_KP5,
  [KEY_KP6] = SDLK_KP6,
  [KEY_KPPLUS] = SDLK_KP_PLUS,
  [KEY_KP1] = SDLK_KP1,
  [KEY_KP2] = SDLK_KP2,
  [KEY_KP3] = SDLK_KP3,
  [KEY_KP0] = SDLK_KP0,
  [KEY_KPDOT] = SDLK_KP_PERIOD,

  [KEY_F11] = SDLK_F11,
  [KEY_F12] = SDLK_F12,
  [KEY_KPENTER] = SDLK_KP_ENTER,
  [KEY_RIGHTCTRL] = SDLK_RCTRL,
  [KEY_KPSLASH] = SDLK_KP_DIVIDE,
  [KEY_SYSRQ] = SDLK_SYSREQ,
  [KEY_RIGHTALT] = SDLK_RALT,
  /*[KEY_LINEFEED] = SDLK_LINEFEED, ??? */
  [KEY_HOME] = SDLK_HOME,
  [KEY_UP] = SDLK_UP,
  [KEY_PAGEUP] = SDLK_PAGEUP,
  [KEY_LEFT] = SDLK_LEFT,
  [KEY_RIGHT] = SDLK_RIGHT,
  [KEY_END] = SDLK_END,
  [KEY_DOWN] = SDLK_DOWN,
  [KEY_PAGEDOWN] = SDLK_PAGEDOWN,
  [KEY_INSERT] = SDLK_INSERT,
  [KEY_DELETE] = SDLK_DELETE,

  [KEY_POWER] = SDLK_POWER,
  [KEY_KPEQUAL] = SDLK_KP_EQUALS,
  [KEY_PAUSE] = SDLK_PAUSE,

  [KEY_LEFTMETA] = SDLK_LMETA,
  [KEY_RIGHTMETA] = SDLK_RMETA,
  [KEY_COMPOSE] = SDLK_COMPOSE,

  [KEY_UNDO] = SDLK_UNDO,
  [KEY_HELP] = SDLK_HELP,
  [KEY_MENU] = SDLK_MENU,

  [KEY_F13] = SDLK_F13,
  [KEY_F14] = SDLK_F14,
  [KEY_F15] = SDLK_F15,

  [KEY_PRINT] = SDLK_PRINT,

  [BTN_LEFT] = SDL_BUTTON_LEFT,
  [BTN_RIGHT] = SDL_BUTTON_RIGHT,
  [BTN_MIDDLE] = SDL_BUTTON_MIDDLE,
};

void
KMSDRM_InitOSKeymap(_THIS)
{
}

static void
KMSDRM_HandleScaling(_THIS)
{
  this->hidden->scaling_mode++;

  if (this->hidden->scaling_mode == DRM_SCALING_MODE_END)
    this->hidden->scaling_mode = 0;
}

static void
KMSDRM_PumpInputDev(_THIS, int fd, const char* path)
{
  struct input_event events[32];
  ssize_t bytes_read;
  SDL_keysym keysym;
  unsigned int i;
  int pressed;

  for (;;) {
    bytes_read = read(fd, events, sizeof(events));
    if (bytes_read < 0) {
      if (errno != EAGAIN)
        SDL_SetError("Unable to read from %s\n", path);
      break;
    }

    for (i = 0; i < bytes_read / sizeof(*events); i++) {
      if (events[i].type == EV_KEY) {
        const char* scaling_key = getenv("SDL_VIDEO_KMSDRM_SCALING_KEY");

        if (scaling_key && events[i].code == atoi(scaling_key)) {
          if (events[i].value)
            KMSDRM_HandleScaling(this);
          continue;
        }

        pressed = events[i].value ? SDL_PRESSED : SDL_RELEASED;
        keysym.sym = keymap[events[i].code];

        if (events[i].code >= BTN_LEFT && events[i].code <= BTN_TASK) {
          /* Mouse button event */
          SDL_PrivateMouseButton(pressed, keysym.sym, 0, 0);
        } else {
          /* Keyboard event */
          SDL_PrivateKeyboard(pressed, &keysym);
        }
      } else if (events[i].type == EV_REL) {
        switch (events[i].code) {
          case REL_X:
            SDL_PrivateMouseMotion(0, SDL_TRUE, events[i].value, 0);
            break;
          case REL_Y:
            SDL_PrivateMouseMotion(0, SDL_TRUE, 0, events[i].value);
            break;
          case REL_WHEEL:
            if (events[i].value < 0) {
              SDL_PrivateMouseButton(SDL_TRUE, SDL_BUTTON_WHEELDOWN, 0, 0);
              SDL_PrivateMouseButton(SDL_FALSE, SDL_BUTTON_WHEELDOWN, 0, 0);
            } else {
              SDL_PrivateMouseButton(SDL_TRUE, SDL_BUTTON_WHEELUP, 0, 0);
              SDL_PrivateMouseButton(SDL_FALSE, SDL_BUTTON_WHEELUP, 0, 0);
            }
            break;
          default:
            break;
        }
      }
    }
  }
}

void
KMSDRM_PumpEvents(_THIS)
{
  drm_input_dev* devs;

  for (devs = this->hidden->keyboards; devs; devs = devs->next)
    KMSDRM_PumpInputDev(this, devs->fd, devs->path);

  for (devs = this->hidden->mice; devs; devs = devs->next)
    KMSDRM_PumpInputDev(this, devs->fd, devs->path);
}

#else

#include "SDL_config.h"

/* Handle the event stream, converting console events into SDL events */

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>

#include <termios.h>
#include <linux/fb.h>

/* For parsing /proc */
#include <dirent.h>
#include <ctype.h>

#include <linux/vt.h>
#include <linux/kd.h>
#include <linux/keyboard.h>

#include "SDL_timer.h"
#include "SDL_mutex.h"
#include "../SDL_sysvideo.h"
#include "../../events/SDL_sysevents.h"
#include "../../events/SDL_events_c.h"
#include "SDL_mutex.h"
#include "SDL_thread.h"
#include "SDL_kmsdrmvideo.h"
#include "SDL_kmsdrmevents_c.h"
#include "../fbcon/SDL_fbkeys.h"

#include "SDL_fbelo.h"
#if SDL_INPUT_TSLIB
#include "tslib.h"
#endif

#ifndef GPM_NODE_FIFO
#define GPM_NODE_FIFO "/dev/gpmdata"
#endif

#define ALT_LAY_LEN SCANCODE_SLASH + 1
static Uint16 utf_alt_lay[2][ALT_LAY_LEN] = { {
                                                // cyrilic
                                                0,
                                                0x1B,   // ESC
                                                0x31,   // 1
                                                0x32,   // 2-"
                                                0x33,   // 3-№
                                                0x34,   // 4-;
                                                0x35,   // 5
                                                0x36,   // 6-:
                                                0x37,   // 7-?
                                                0x38,   // 8
                                                0x39,   // 9
                                                0x30,   // 0
                                                0x2D,   //-
                                                0x3D,   //=
                                                0x8,    // BS
                                                0x9,    // TAB
                                                0x0439, // q-й
                                                0x0446, // ц
                                                0x0443, // у
                                                0x043A, // к
                                                0x0435, // е
                                                0x043D, // н
                                                0x0433, // г
                                                0x0448, // ш
                                                0x0449, // щ
                                                0x0437, // p-з
                                                0x0445, //[-х
                                                0x044A, //]-ъ
                                                0xD,    // ENTER
                                                0,      // LEFTCONTROL
                                                0x0444, // a-ф
                                                0x044B, // ы
                                                0x0432, // в
                                                0x0430, // а
                                                0x043F, // п
                                                0x0440, // р
                                                0x043E, // о
                                                0x043B, // л
                                                0x0434, // l-д
                                                0x0436, //;-ж
                                                0x044D, //'-э
                                                0x0451, //`-ё
                                                0,      // LEFTSHIFT
                                                0x2F,   // BACKSLASH-SLASH
                                                0x044F, // z-я
                                                0x0447, // ч
                                                0x0441, // с
                                                0x043C, // м
                                                0x0438, // и
                                                0x0442, // т
                                                0x044C, // m-ь
                                                0x0431, //,-б
                                                0x044E, //.-ю
                                                0x2E    // SLASH-.
                                              },
                                              {
                                                0,
                                                0x1B,   // ESC
                                                0x21,   // 1
                                                0x22,   // 2-"
                                                0x2116, // 3-№
                                                0x3B,   // 4-;
                                                0x25,   // 5
                                                0x3A,   // 6-:
                                                0x3F,   // 7-?
                                                0x2A,   // 8
                                                0x28,   // 9
                                                0x29,   // 0
                                                0x5F,   //-
                                                0x2B,   //=
                                                0x8,    // BS
                                                0x9,    // TAB
                                                0x0419, // Q-Й
                                                0x0426, // Ц
                                                0x0423, // У
                                                0x041A, // К
                                                0x0415, // Е
                                                0x041D, // Н
                                                0x0413, // Г
                                                0x0428, // Ш
                                                0x0429, // Щ
                                                0x0417, // P-З
                                                0x0425, //{-Х
                                                0x042A, //}-Ъ
                                                0xD,    // ENTER
                                                0,      // LEFTCONTROL
                                                0x0424, // A-Ф
                                                0x042B, // Ы
                                                0x0412, // В
                                                0x0410, // А
                                                0x041F, // П
                                                0x0420, // Р
                                                0x041E, // О
                                                0x041B, // Л
                                                0x0414, // L-Д
                                                0x0416, //:-Ж
                                                0x042D, //"-Э
                                                0x0401, //~-Ё
                                                0,      // LEFTSHIFT
                                                0x7C,   //|
                                                0x042F, // Z-Я
                                                0x0427, // Ч
                                                0x0421, // С
                                                0x041C, // М
                                                0x0418, // И
                                                0x0422, // Т
                                                0x042C, // M-Ь
                                                0x0411, //<-Б
                                                0x042E, //>-Ю
                                                0x2C    //?-,
                                              } };

void FB_CloseKeyboard(_THIS);
void FB_CloseMouse(_THIS);

// #define DEBUG_KEYBOARD
// #define DEBUG_MOUSE

static char KeybLEDsState;

static void
setKeybLEDs(_THIS)
{
  // if (ioctl(keyboard_fd, KDGETLED, &kleds) == 0) {
  ioctl(keyboard_fd, KDSETLED, KeybLEDsState);
  //}
}

/* The translation tables from a console scancode to a SDL keysym */
#define NUM_VGAKEYMAPS (1 << KG_CAPSSHIFT)
static Uint16 vga_keymap[NUM_VGAKEYMAPS][NR_KEYS];
static SDLKey keymap[128];
static unsigned char KeybAltLayout;
static Uint16 keymap_temp[128]; /* only used at startup */
static SDL_keysym*
TranslateKey(int scancode, SDL_keysym* keysym, SDLMod modstate);

/* Ugh, we have to duplicate the kernel's keysym mapping code...
   Oh, it's not so bad. :-)

   FIXME: Add keyboard LED handling code
 */
static void
FB_vgainitkeymaps(int fd)
{
  struct kbentry entry;
  int map, i;

  KeybAltLayout = 0;
  /* Don't do anything if we are passed a closed keyboard */
  if (fd < 0) {
    return;
  }

  /* Load all the keysym mappings */
  for (map = 0; map < NUM_VGAKEYMAPS; ++map) {
    SDL_memset(vga_keymap[map], 0, NR_KEYS * sizeof(Uint16));
    for (i = 0; i < NR_KEYS; ++i) {
      entry.kb_table = map;
      entry.kb_index = i;
      if (ioctl(fd, KDGKBENT, &entry) == 0) {
        /* fill keytemp. This replaces SDL_fbkeys.h */
        if ((map == 0) && (i < 128)) {
          keymap_temp[i] = entry.kb_value;
        }
        /* The "Enter" key is a special case */
        if (entry.kb_value == K_ENTER) {
          entry.kb_value = K(KT_ASCII, 13);
        }
        /* Handle numpad specially as well */
        if (KTYP(entry.kb_value) == KT_PAD) {
          switch (entry.kb_value) {
            case K_P0:
            case K_P1:
            case K_P2:
            case K_P3:
            case K_P4:
            case K_P5:
            case K_P6:
            case K_P7:
            case K_P8:
            case K_P9:
              vga_keymap[map][i] = entry.kb_value;
              vga_keymap[map][i] += '0';
              break;
            case K_PPLUS:
              vga_keymap[map][i] = K(KT_ASCII, '+');
              break;
            case K_PMINUS:
              vga_keymap[map][i] = K(KT_ASCII, '-');
              break;
            case K_PSTAR:
              vga_keymap[map][i] = K(KT_ASCII, '*');
              break;
            case K_PSLASH:
              vga_keymap[map][i] = K(KT_ASCII, '/');
              break;
            case K_PENTER:
              vga_keymap[map][i] = K(KT_ASCII, '\r');
              break;
            case K_PCOMMA:
              vga_keymap[map][i] = K(KT_ASCII, ',');
              break;
            case K_PDOT:
              vga_keymap[map][i] = K(KT_ASCII, '.');
              break;
            default:
              break;
          }
        }
        /* Do the normal key translation */
        if ((KTYP(entry.kb_value) == KT_LATIN) ||
            (KTYP(entry.kb_value) == KT_ASCII) ||
            (KTYP(entry.kb_value) == KT_LETTER)) {
          vga_keymap[map][i] = entry.kb_value;
        }
      }
    }
  }
}

int
FB_InGraphicsMode(_THIS)
{
  return ((keyboard_fd >= 0) && (saved_kbd_mode >= 0));
}

int
FB_EnterGraphicsMode(_THIS)
{
  struct termios keyboard_termios;

  /* Set medium-raw keyboard mode */
  if ((keyboard_fd >= 0) && !FB_InGraphicsMode(this)) {

    /* Switch to the correct virtual terminal */
    if (current_vt > 0) {
      struct vt_stat vtstate;

      if (ioctl(keyboard_fd, VT_GETSTATE, &vtstate) == 0) {
        saved_vt = vtstate.v_active;
      }
      if (ioctl(keyboard_fd, VT_ACTIVATE, current_vt) == 0) {
        ioctl(keyboard_fd, VT_WAITACTIVE, current_vt);
      }
    }

    /* Set the terminal input mode */
    if (tcgetattr(keyboard_fd, &saved_kbd_termios) < 0) {
      SDL_SetError("Unable to get terminal attributes");
      if (keyboard_fd > 0) {
        close(keyboard_fd);
      }
      keyboard_fd = -1;
      return (-1);
    }
    if (ioctl(keyboard_fd, KDGKBMODE, &saved_kbd_mode) < 0) {
      SDL_SetError("Unable to get current keyboard mode");
      if (keyboard_fd > 0) {
        close(keyboard_fd);
      }
      keyboard_fd = -1;
      return (-1);
    }
    keyboard_termios = saved_kbd_termios;
    keyboard_termios.c_lflag &= ~(ICANON | ECHO | ISIG);
    keyboard_termios.c_iflag &=
      ~(ISTRIP | IGNCR | ICRNL | INLCR | IXOFF | IXON);
    keyboard_termios.c_cc[VMIN] = 0;
    keyboard_termios.c_cc[VTIME] = 0;
    if (tcsetattr(keyboard_fd, TCSAFLUSH, &keyboard_termios) < 0) {
      FB_CloseKeyboard(this);
      SDL_SetError("Unable to set terminal attributes");
      return (-1);
    }
    /* This will fail if we aren't root or this isn't our tty */
    if (ioctl(keyboard_fd, KDSKBMODE, K_MEDIUMRAW) < 0) {
      FB_CloseKeyboard(this);
      SDL_SetError("Unable to set keyboard in raw mode");
      return (-1);
    }
    /*
        if (ioctl(keyboard_fd, KDSETMODE, KD_GRAPHICS) < 0) {
          FB_CloseKeyboard(this);
          SDL_SetError("Unable to set keyboard in graphics mode");
          return (-1);
        }
    */
    /* Prevent switching the virtual terminal */
    // ioctl(keyboard_fd, VT_LOCKSWITCH, 1);

    KeybLEDsState = 0;
    setKeybLEDs(this);
  }
  return (keyboard_fd);
}

void
FB_LeaveGraphicsMode(_THIS)
{
  if (FB_InGraphicsMode(this)) {
    ioctl(keyboard_fd, KDSETMODE, KD_TEXT);
    ioctl(keyboard_fd, KDSKBMODE, saved_kbd_mode);
    tcsetattr(keyboard_fd, TCSAFLUSH, &saved_kbd_termios);
    saved_kbd_mode = -1;

    /* Head back over to the original virtual terminal */
    // ioctl(keyboard_fd, VT_UNLOCKSWITCH, 1);
    if (saved_vt > 0) {
      ioctl(keyboard_fd, VT_ACTIVATE, saved_vt);
    }
  }
}

void
FB_CloseKeyboard(_THIS)
{
  if (keyboard_fd >= 0) {
    FB_LeaveGraphicsMode(this);
    if (keyboard_fd > 0) {
      close(keyboard_fd);
    }
  }
  keyboard_fd = -1;
}

int
FB_OpenKeyboard(_THIS)
{
  /* Open only if not already opened */
  if (keyboard_fd < 0) {
    static const char* const tty0[] = { "/dev/tty0", "/dev/vc/0", NULL };
    static const char* const vcs[] = { "/dev/vc/%d", "/dev/tty%d", NULL };
    int i, tty0_fd;

    /* Set as session leader if it's not yet the case */
    setsid();

    /* Try to query for a free virtual terminal */
    tty0_fd = -1;
    for (i = 0; tty0[i] && (tty0_fd < 0); ++i) {
      tty0_fd = open(tty0[i], O_WRONLY, 0);
    }
    if (tty0_fd < 0) {
      tty0_fd = dup(0); /* Maybe stdin is a VT? */
    }
    ioctl(tty0_fd, VT_OPENQRY, &current_vt);
    close(tty0_fd);
    if (current_vt > 0) {
      for (i = 0; vcs[i] && (keyboard_fd < 0); ++i) {
        char vtpath[12];

        SDL_snprintf(vtpath, SDL_arraysize(vtpath), vcs[i], current_vt);
        keyboard_fd = open(vtpath, O_RDWR, 0);
#ifdef DEBUG_KEYBOARD
        fprintf(stderr, "vtpath = %s, fd = %d\n", vtpath, keyboard_fd);
#endif /* DEBUG_KEYBOARD */
      }
    }
    if (keyboard_fd < 0) {
      /* Last resort, maybe our tty is a usable VT */
      struct vt_stat vtstate;

      keyboard_fd = open("/dev/tty", O_RDWR);

      if (ioctl(keyboard_fd, VT_GETSTATE, &vtstate) == 0) {
        current_vt = vtstate.v_active;
      } else {
        current_vt = 0;
      }
    }
#ifdef DEBUG_KEYBOARD
    fprintf(stderr, "Current VT: %d\n", current_vt);
#endif
    saved_kbd_mode = -1;

    /* Make sure that our input is a console terminal */
    {
      int dummy;
      if (ioctl(keyboard_fd, KDGKBMODE, &dummy) < 0) {
        close(keyboard_fd);
        keyboard_fd = -1;
        SDL_SetError("Unable to open a console terminal");
      }
    }

    /* Set up keymap */
    FB_vgainitkeymaps(keyboard_fd);
  }
  return (keyboard_fd);
}

static enum {
  MOUSE_NONE = -1,
  MOUSE_MSC, /* Note: GPM uses the MSC protocol */
  MOUSE_PS2,
  MOUSE_IMPS2,
  MOUSE_MS,
  MOUSE_BM,
  MOUSE_ELO,
  MOUSE_TSLIB,
  NUM_MOUSE_DRVS
} mouse_drv = MOUSE_NONE;

void
FB_CloseMouse(_THIS)
{
#if SDL_INPUT_TSLIB
  if (ts_dev != NULL) {
    ts_close(ts_dev);
    ts_dev = NULL;
    // mouse_fd = -1;
  }
#endif /* SDL_INPUT_TSLIB */
  if (mouse_fd > 0) {
    close(mouse_fd);
  }
  mouse_fd = -1;
}

/* Returns processes listed in /proc with the desired name */
static int
find_pid(DIR* proc, const char* wanted_name)
{
  struct dirent* entry;
  int pid;

  /* First scan proc for the gpm process */
  pid = 0;
  while ((pid == 0) && ((entry = readdir(proc)) != NULL)) {
    if (isdigit(entry->d_name[0])) {
      FILE* status;
      char path[PATH_MAX];
      char name[PATH_MAX];

      SDL_snprintf(path, SDL_arraysize(path), "/proc/%s/status", entry->d_name);
      status = fopen(path, "r");
      if (status) {
        int matches = 0;
        name[0] = '\0';
        matches = fscanf(status, "Name: %s", name);
        if ((matches == 1) && (SDL_strcmp(name, wanted_name) == 0)) {
          pid = SDL_atoi(entry->d_name);
        }
        fclose(status);
      }
    }
  }
  return pid;
}

/* Returns true if /dev/gpmdata is being written to by gpm */
static int
gpm_available(char* proto, size_t protolen)
{
  int available;
  DIR* proc;
  int pid;
  int cmdline, len, arglen;
  char path[PATH_MAX];
  char args[PATH_MAX], *arg;

  /* Don't bother looking if the fifo isn't there */
#ifdef DEBUG_MOUSE
  fprintf(stderr, "testing gpm\n");
#endif
  if (access(GPM_NODE_FIFO, F_OK) < 0) {
    return (0);
  }

  available = 0;
  proc = opendir("/proc");
  if (proc) {
    char raw_proto[10] = { '\0' };
    char repeat_proto[10] = { '\0' };
    while (!available && (pid = find_pid(proc, "gpm")) > 0) {
      SDL_snprintf(path, SDL_arraysize(path), "/proc/%d/cmdline", pid);
      cmdline = open(path, O_RDONLY, 0);
      if (cmdline >= 0) {
        len = read(cmdline, args, sizeof(args));
        arg = args;
        while (len > 0) {
          arglen = SDL_strlen(arg) + 1;
#ifdef DEBUG_MOUSE
          fprintf(stderr, "gpm arg %s len %d\n", arg, arglen);
#endif
          if (SDL_strcmp(arg, "-t") == 0) {
            /* protocol string, keep it for later */
            char *t, *s;
            t = arg + arglen;
            s = SDL_strchr(t, ' ');
            if (s)
              *s = 0;
            SDL_strlcpy(raw_proto, t, SDL_arraysize(raw_proto));
            if (s)
              *s = ' ';
          }
          if (SDL_strncmp(arg, "-R", 2) == 0) {
            char *t, *s;
            available = 1;
            t = arg + 2;
            s = SDL_strchr(t, ' ');
            if (s)
              *s = 0;
            SDL_strlcpy(repeat_proto, t, SDL_arraysize(repeat_proto));
            if (s)
              *s = ' ';
          }
          len -= arglen;
          arg += arglen;
        }
        close(cmdline);
      }
    }
    closedir(proc);

    if (available) {
      if (SDL_strcmp(repeat_proto, "raw") == 0) {
        SDL_strlcpy(proto, raw_proto, protolen);
      } else if (*repeat_proto) {
        SDL_strlcpy(proto, repeat_proto, protolen);
      } else {
        SDL_strlcpy(proto, "msc", protolen);
      }
    }
  }
  return available;
}

/* rcg06112001 Set up IMPS/2 mode, if possible. This gives
 *  us access to the mousewheel, etc. Returns zero if
 *  writes to device failed, but you still need to query the
 *  device to see which mode it's actually in.
 */
static int
set_imps2_mode(int fd)
{
  /* If you wanted to control the mouse mode (and we do :)  ) ...
          Set IMPS/2 protocol:
                  {0xf3,200,0xf3,100,0xf3,80}
          Reset mouse device:
                  {0xFF}
  */
  Uint8 set_imps2[] = { 0xf3, 200, 0xf3, 100, 0xf3, 80 };
  /*Uint8 reset = 0xff;*/
  fd_set fdset;
  struct timeval tv;
  int retval = 0;

  if (write(fd, &set_imps2, sizeof(set_imps2)) == sizeof(set_imps2)) {
    /* Don't reset it, that'll clear IMPS/2 mode on some mice
    if (write(fd, &reset, sizeof (reset)) == sizeof (reset) ) {
            retval = 1;
    }
    */
  }

  /* Get rid of any chatter from the above */
  FD_ZERO(&fdset);
  FD_SET(fd, &fdset);
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  while (select(fd + 1, &fdset, 0, 0, &tv) > 0) {
    char temp[32];
    if (read(fd, temp, sizeof(temp)) <= 0) {
      break;
    }
  }

  return retval;
}

/* Returns true if the mouse uses the IMPS/2 protocol */
static int
detect_imps2(int fd)
{
  int imps2;

  imps2 = 0;

  if (SDL_getenv("SDL_MOUSEDEV_IMPS2")) {
    imps2 = 1;
  }
  if (!imps2) {
    Uint8 query_ps2 = 0xF2;
    fd_set fdset;
    struct timeval tv;

    /* Get rid of any mouse motion noise */
    FD_ZERO(&fdset);
    FD_SET(fd, &fdset);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    while (select(fd + 1, &fdset, 0, 0, &tv) > 0) {
      char temp[32];
      if (read(fd, temp, sizeof(temp)) <= 0) {
        break;
      }
    }

    /* Query for the type of mouse protocol */
    if (write(fd, &query_ps2, sizeof(query_ps2)) == sizeof(query_ps2)) {
      Uint8 ch = 0;

      /* Get the mouse protocol response */
      do {
        FD_ZERO(&fdset);
        FD_SET(fd, &fdset);
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        if (select(fd + 1, &fdset, 0, 0, &tv) < 1) {
          break;
        }
      } while ((read(fd, &ch, sizeof(ch)) == sizeof(ch)) &&
               ((ch == 0xFA) || (ch == 0xAA)));

      /* Experimental values (Logitech wheelmouse) */
#ifdef DEBUG_MOUSE
      fprintf(stderr, "Last mouse mode: 0x%x\n", ch);
#endif
      if ((ch == 3) || (ch == 4)) {
        imps2 = 1;
      }
    }
  }
  return imps2;
}

int
FB_OpenMouse(_THIS)
{
  int i;
  const char* mousedev;
  const char* mousedrv;

  mousedrv = SDL_getenv("SDL_MOUSEDRV");
  mousedev = SDL_getenv("SDL_MOUSEDEV");
  mouse_fd = -1;

#if SDL_INPUT_TSLIB
  if (mousedrv && (SDL_strcmp(mousedrv, "TSLIB") == 0)) {
    if (mousedev == NULL)
      mousedev = SDL_getenv("TSLIB_TSDEVICE");
    if (mousedev != NULL) {
      ts_dev = ts_open(mousedev, 1);
      if ((ts_dev != NULL) && (ts_config(ts_dev) >= 0)) {
#ifdef DEBUG_MOUSE
        fprintf(stderr, "Using tslib touchscreen\n");
#endif
        mouse_drv = MOUSE_TSLIB;
        mouse_fd = ts_fd(ts_dev);
        return mouse_fd;
      }
    }
    mouse_drv = MOUSE_NONE;
    return mouse_fd;
  }
#endif /* SDL_INPUT_TSLIB */

  /* ELO TOUCHSCREEN SUPPORT */

  if (mousedrv && (SDL_strcmp(mousedrv, "ELO") == 0)) {
    mouse_fd = open(mousedev, O_RDWR);
    if (mouse_fd >= 0) {
      if (eloInitController(mouse_fd)) {
#ifdef DEBUG_MOUSE
        fprintf(stderr, "Using ELO touchscreen\n");
#endif
        mouse_drv = MOUSE_ELO;
      }

    } else if (mouse_fd < 0) {
      mouse_drv = MOUSE_NONE;
    }

    return (mouse_fd);
  }

  /* STD MICE */

  if (mousedev == NULL) {
    /* FIXME someday... allow multiple mice in this driver */
    static const char* ps2mice[] = {
      "/dev/input/mice", "/dev/usbmouse", "/dev/psaux", NULL
    };
    /* First try to use GPM in repeater mode */
    if (mouse_fd < 0) {
      char proto[10];
      if (gpm_available(proto, SDL_arraysize(proto))) {
        mouse_fd = open(GPM_NODE_FIFO, O_RDONLY, 0);
        if (mouse_fd >= 0) {
          if (SDL_strcmp(proto, "msc") == 0) {
            mouse_drv = MOUSE_MSC;
          } else if (SDL_strcmp(proto, "ps2") == 0) {
            mouse_drv = MOUSE_PS2;
          } else if (SDL_strcmp(proto, "imps2") == 0) {
            mouse_drv = MOUSE_IMPS2;
          } else if (SDL_strcmp(proto, "ms") == 0 ||
                     SDL_strcmp(proto, "bare") == 0) {
            mouse_drv = MOUSE_MS;
          } else if (SDL_strcmp(proto, "bm") == 0) {
            mouse_drv = MOUSE_BM;
          } else {
            /* Unknown protocol... */
#ifdef DEBUG_MOUSE
            fprintf(stderr, "GPM mouse using unknown protocol = %s\n", proto);
#endif
            close(mouse_fd);
            mouse_fd = -1;
          }
        }
#ifdef DEBUG_MOUSE
        if (mouse_fd >= 0) {
          fprintf(stderr, "Using GPM mouse, protocol = %s\n", proto);
        }
#endif /* DEBUG_MOUSE */
      }
    }
    /* Now try to use a modern PS/2 mouse */
    for (i = 0; (mouse_fd < 0) && ps2mice[i]; ++i) {
      mouse_fd = open(ps2mice[i], O_RDWR, 0);
      if (mouse_fd < 0) {
        mouse_fd = open(ps2mice[i], O_RDONLY, 0);
      }
      if (mouse_fd >= 0) {
        /* rcg06112001 Attempt to set IMPS/2 mode */
        set_imps2_mode(mouse_fd);
        if (detect_imps2(mouse_fd)) {
#ifdef DEBUG_MOUSE
          fprintf(stderr, "Using IMPS2 mouse\n");
#endif
          mouse_drv = MOUSE_IMPS2;
        } else {
#ifdef DEBUG_MOUSE
          fprintf(stderr, "Using PS2 mouse\n");
#endif
          mouse_drv = MOUSE_PS2;
        }
      }
    }
    /* Next try to use a PPC ADB port mouse */
    if (mouse_fd < 0) {
      mouse_fd = open("/dev/adbmouse", O_RDONLY, 0);
      if (mouse_fd >= 0) {
#ifdef DEBUG_MOUSE
        fprintf(stderr, "Using ADB mouse\n");
#endif
        mouse_drv = MOUSE_BM;
      }
    }
  }
  /* Default to a serial Microsoft mouse */
  if (mouse_fd < 0) {
    if (mousedev == NULL) {
      mousedev = "/dev/mouse";
    }
    mouse_fd = open(mousedev, O_RDONLY, 0);
    if (mouse_fd >= 0) {
      struct termios mouse_termios;

      /* Set the sampling speed to 1200 baud */
      tcgetattr(mouse_fd, &mouse_termios);
      mouse_termios.c_iflag = IGNBRK | IGNPAR;
      mouse_termios.c_oflag = 0;
      mouse_termios.c_lflag = 0;
      mouse_termios.c_line = 0;
      mouse_termios.c_cc[VTIME] = 0;
      mouse_termios.c_cc[VMIN] = 1;
      mouse_termios.c_cflag = CREAD | CLOCAL | HUPCL;
      mouse_termios.c_cflag |= CS8;
      mouse_termios.c_cflag |= B1200;
      tcsetattr(mouse_fd, TCSAFLUSH, &mouse_termios);
      if (mousedrv && (SDL_strcmp(mousedrv, "PS2") == 0)) {
#ifdef DEBUG_MOUSE
        fprintf(stderr, "Using (user specified) PS2 mouse on %s\n", mousedev);
#endif
        mouse_drv = MOUSE_PS2;
      } else {
#ifdef DEBUG_MOUSE
        fprintf(stderr, "Using (default) MS mouse on %s\n", mousedev);
#endif
        mouse_drv = MOUSE_MS;
      }
    }
  }
  if (mouse_fd < 0) {
    mouse_drv = MOUSE_NONE;
  }
  return (mouse_fd);
}

static int posted = 0;

void
FB_vgamousecallback(int button, int relative, int dx, int dy)
{
  int button_1, button_3;
  int button_state;
  int state_changed;
  int i;
  Uint8 state;

  if (dx || dy) {
    posted += SDL_PrivateMouseMotion(0, relative, dx, dy);
  }

  /* Swap button 1 and 3 */
  button_1 = (button & 0x04) >> 2;
  button_3 = (button & 0x01) << 2;
  button &= ~0x05;
  button |= (button_1 | button_3);

  /* See what changed */
  button_state = SDL_GetMouseState(NULL, NULL);
  state_changed = button_state ^ button;
  for (i = 0; i < 8; ++i) {
    if (state_changed & (1 << i)) {
      if (button & (1 << i)) {
        state = SDL_PRESSED;
      } else {
        state = SDL_RELEASED;
      }
      posted += SDL_PrivateMouseButton(state, i + 1, 0, 0);
    }
  }
}

/* Handle input from tslib */
#if SDL_INPUT_TSLIB
static void
handle_tslib(_THIS)
{
  struct ts_sample sample;
  int button;

  while (ts_read(ts_dev, &sample, 1) > 0) {
    button = (sample.pressure > 0) ? 1 : 0;
    button <<= 2; /* must report it as button 3 */
    FB_vgamousecallback(button, 0, sample.x, sample.y);
  }
  return;
}
#endif /* SDL_INPUT_TSLIB */

/* For now, use MSC, PS/2, and MS protocols
   Driver adapted from the SVGAlib mouse driver code (taken from gpm, etc.)
 */
static void
handle_mouse(_THIS)
{
  static int start = 0;
  static unsigned char mousebuf[BUFSIZ];
  static int relative = 1;

  int i, nread;
  int button = 0;
  int dx = 0, dy = 0;
  int packetsize = 0;
  int realx, realy;

  /* Figure out the mouse packet size */
  switch (mouse_drv) {
    case MOUSE_NONE:
      break; /* carry on to read from device and discard it. */
    case MOUSE_MSC:
      packetsize = 5;
      break;
    case MOUSE_IMPS2:
      packetsize = 4;
      break;
    case MOUSE_PS2:
    case MOUSE_MS:
    case MOUSE_BM:
      packetsize = 3;
      break;
    case MOUSE_ELO:
      /* try to read the next packet */
      if (eloReadPosition(this, mouse_fd, &dx, &dy, &button, &realx, &realy)) {
        button = (button & 0x01) << 2;
        FB_vgamousecallback(button, 0, dx, dy);
      }
      return; /* nothing left to do */
    case MOUSE_TSLIB:
#if SDL_INPUT_TSLIB
      handle_tslib(this);
#endif
      return; /* nothing left to do */
    default:
      /* Uh oh.. */
      packetsize = 0;
      break;
  }

  /* Special handling for the quite sensitive ELO controller */
  if (mouse_drv == MOUSE_ELO) {
  }

  /* Read as many packets as possible */
  nread = read(mouse_fd, &mousebuf[start], BUFSIZ - start);
  if (nread < 0) {
    return;
  }

  if (mouse_drv == MOUSE_NONE) {
    return; /* we're done; just draining the input queue. */
  }

  nread += start;
#ifdef DEBUG_MOUSE
  fprintf(stderr, "Read %d bytes from mouse, start = %d\n", nread, start);
#endif

  for (i = 0; i < (nread - (packetsize - 1)); i += packetsize) {
    switch (mouse_drv) {
      case MOUSE_NONE: /* shouldn't actually hit this. */
        break;         /* just throw everything away. */
      case MOUSE_MSC:
        /* MSC protocol has 0x80 in high byte */
        if ((mousebuf[i] & 0xF8) != 0x80) {
          /* Go to next byte */
          i -= (packetsize - 1);
          continue;
        }
        /* Get current mouse state */
        button = (~mousebuf[i]) & 0x07;
        dx = (signed char)(mousebuf[i + 1]) + (signed char)(mousebuf[i + 3]);
        dy = -((signed char)(mousebuf[i + 2]) + (signed char)(mousebuf[i + 4]));
        break;
      case MOUSE_PS2:
        /* PS/2 protocol has nothing in high byte */
        if ((mousebuf[i] & 0xC0) != 0) {
          /* Go to next byte */
          i -= (packetsize - 1);
          continue;
        }
        /* Get current mouse state */
        button = (mousebuf[i] & 0x04) >> 1 | /*Middle*/
                 (mousebuf[i] & 0x02) >> 1 | /*Right*/
                 (mousebuf[i] & 0x01) << 2;  /*Left*/
        dx = (mousebuf[i] & 0x10) ? mousebuf[i + 1] - 256 : mousebuf[i + 1];
        dy = (mousebuf[i] & 0x20) ? -(mousebuf[i + 2] - 256) : -mousebuf[i + 2];
        break;
      case MOUSE_IMPS2:
        /* Get current mouse state */
        button = (mousebuf[i] & 0x04) >> 1 | /*Middle*/
                 (mousebuf[i] & 0x02) >> 1 | /*Right*/
                 (mousebuf[i] & 0x01) << 2 | /*Left*/
                 (mousebuf[i] & 0x40) >> 3 | /* 4 */
                 (mousebuf[i] & 0x80) >> 3;  /* 5 */
        dx = (mousebuf[i] & 0x10) ? mousebuf[i + 1] - 256 : mousebuf[i + 1];
        dy = (mousebuf[i] & 0x20) ? -(mousebuf[i + 2] - 256) : -mousebuf[i + 2];
        switch (mousebuf[i + 3] & 0x0F) {
          case 0x0E: /* DX = +1 */
          case 0x02: /* DX = -1 */
            break;
          case 0x0F: /* DY = +1 (map button 4) */
            FB_vgamousecallback(button | (1 << 3), 1, 0, 0);
            break;
          case 0x01: /* DY = -1 (map button 5) */
            FB_vgamousecallback(button | (1 << 4), 1, 0, 0);
            break;
        }
        break;
      case MOUSE_MS:
        /* Microsoft protocol has 0x40 in high byte */
        if ((mousebuf[i] & 0x40) != 0x40) {
          /* Go to next byte */
          i -= (packetsize - 1);
          continue;
        }
        /* Get current mouse state */
        button = ((mousebuf[i] & 0x20) >> 3) | ((mousebuf[i] & 0x10) >> 4);
        dx =
          (signed char)(((mousebuf[i] & 0x03) << 6) | (mousebuf[i + 1] & 0x3F));
        dy =
          (signed char)(((mousebuf[i] & 0x0C) << 4) | (mousebuf[i + 2] & 0x3F));
        break;
      case MOUSE_BM:
        /* BusMouse protocol has 0xF8 in high byte */
        if ((mousebuf[i] & 0xF8) != 0x80) {
          /* Go to next byte */
          i -= (packetsize - 1);
          continue;
        }
        /* Get current mouse state */
        button = (~mousebuf[i]) & 0x07;
        dx = (signed char)mousebuf[i + 1];
        dy = -(signed char)mousebuf[i + 2];
        break;
      default:
        /* Uh oh.. */
        dx = 0;
        dy = 0;
        break;
    }
    FB_vgamousecallback(button, relative, dx, dy);
  }
  if (i < nread) {
    SDL_memcpy(mousebuf, &mousebuf[i], (nread - i));
    start = (nread - i);
  } else {
    start = 0;
  }
  return;
}

/* Handle switching to another VC, returns when our VC is back */
static void
switch_vt_prep(_THIS)
{
  SDL_PrivateAppActive(0,
                       (SDL_APPACTIVE | SDL_APPINPUTFOCUS | SDL_APPMOUSEFOCUS));

  /* Save the contents of the screen, and go to text mode */
  ioctl(keyboard_fd, KDSETMODE, KD_TEXT);
  // ioctl(keyboard_fd, VT_UNLOCKSWITCH, 1);
}

static void
switch_vt_done(_THIS)
{
  printf("Restore console\n");
  /* Restore graphics mode and the contents of the screen */
  // ioctl(keyboard_fd, VT_LOCKSWITCH, 1);
  // ioctl(keyboard_fd, KDSETMODE, KD_GRAPHICS);

  /* Get updates to the shadow surface while switched away
  if (SDL_ShadowSurface) {
    SDL_UpdateRect(SDL_ShadowSurface, 0, 0, 0, 0);
  }
*/
  KMSDRM_ShowLayer(this, 1);
  SDL_PrivateAppActive(1,
                       (SDL_APPACTIVE | SDL_APPINPUTFOCUS | SDL_APPMOUSEFOCUS));
}

static void
switch_vt(_THIS, unsigned short which)
{
  struct vt_stat vtstate;

  /* Figure out whether or not we're switching to a new console */
  if ((ioctl(keyboard_fd, VT_GETSTATE, &vtstate) < 0) ||
      (which == vtstate.v_active)) {
    return;
  }

  printf("New console\n");
  /* New console, switch to it */
  // SDL_mutexP(hw_lock);
  switch_vt_prep(this);
  if (ioctl(keyboard_fd, VT_ACTIVATE, which) == 0) {
    ioctl(keyboard_fd, VT_WAITACTIVE, which);
    switched_away = 1;
    KMSDRM_ShowLayer(this, 0);
  } else {
    switch_vt_done(this);
  }
  // SDL_mutexV(hw_lock);
}

static void
handle_keyboard(_THIS)
{
  unsigned char keybuf[BUFSIZ];
  int i, nread;
  int pressed;
  int scancode;
  SDL_keysym keysym;

  SDLMod kmod = SDL_GetModState();
  nread = read(keyboard_fd, keybuf, BUFSIZ);
  for (i = 0; i < nread; ++i) {
    scancode = keybuf[i] & 0x7F;
    if (keybuf[i] & 0x80) {
      pressed = SDL_RELEASED;
    } else {
      pressed = SDL_PRESSED;
      // printf("scan:%d  mod:% \n");
      char kl_ch = 0;
      switch (scancode) {
        case SCANCODE_RIGHTSHIFT:
          if (kmod & KMOD_RCTRL) {
            kl_ch = 1;
          }
          break;
        case SCANCODE_RIGHTCONTROL:
          if (kmod & KMOD_RSHIFT) {
            kl_ch = 1;
          }
          break;
        case SCANCODE_CAPSLOCK:
          if (kmod & KMOD_CAPS) {
            KeybLEDsState &= ~LED_CAP;
          } else {
            KeybLEDsState |= LED_CAP;
          }
          setKeybLEDs(this);
          break;
      }
      if (kl_ch) {
        KeybAltLayout = !KeybAltLayout;
        if (KeybAltLayout) {
          KeybLEDsState |= LED_SCR;
        } else {
          KeybLEDsState &= ~LED_SCR;
        }
        setKeybLEDs(this);
      }
    }

    TranslateKey(scancode, &keysym, kmod);
    /* Handle Ctrl-Alt-FN for vt switch */
    switch (keysym.sym) {
      case SDLK_F1:
      case SDLK_F2:
      case SDLK_F3:
      case SDLK_F4:
      case SDLK_F5:
      case SDLK_F6:
      case SDLK_F7:
      case SDLK_F8:
      case SDLK_F9:
      case SDLK_F10:
      case SDLK_F11:
      case SDLK_F12:
        if ((kmod & KMOD_CTRL) && (kmod & KMOD_ALT)) {
          if (pressed) {
            switch_vt(this, (keysym.sym - SDLK_F1) + 1);
          }
          break;
        }
        /* Fall through to normal processing */
      default:
        posted += SDL_PrivateKeyboard(pressed, &keysym);
        break;
    }
  }
}

void
KMSDRM_PumpEvents(_THIS)
{
  fd_set fdset;
  int max_fd;
  static struct timeval zero;

  do {
    if (switched_away) {
      struct vt_stat vtstate;

      // SDL_mutexP(hw_lock);
      if ((ioctl(keyboard_fd, VT_GETSTATE, &vtstate) == 0) &&
          vtstate.v_active == current_vt) {
        switched_away = 0;
        switch_vt_done(this);
      }
      // SDL_mutexV(hw_lock);
    }

    posted = 0;

    FD_ZERO(&fdset);
    max_fd = 0;
    if (keyboard_fd >= 0) {
      FD_SET(keyboard_fd, &fdset);
      if (max_fd < keyboard_fd) {
        max_fd = keyboard_fd;
      }
    }
    if (mouse_fd >= 0) {
      FD_SET(mouse_fd, &fdset);
      if (max_fd < mouse_fd) {
        max_fd = mouse_fd;
      }
    }
    if (select(max_fd + 1, &fdset, NULL, NULL, &zero) > 0) {
      if (keyboard_fd >= 0) {
        if (FD_ISSET(keyboard_fd, &fdset)) {
          handle_keyboard(this);
        }
      }
      if (mouse_fd >= 0) {
        if (FD_ISSET(mouse_fd, &fdset)) {
          handle_mouse(this);
        }
      }
    }
  } while (posted);
}

void
KMSDRM_InitOSKeymap(_THIS)
{
  int i;

  /* Initialize the Linux key translation table */

  /* First get the ascii keys and others not well handled */
  for (i = 0; i < SDL_arraysize(keymap); ++i) {
    switch (i) {
      /* These aren't handled by the x86 kernel keymapping (?) */
      case SCANCODE_PRINTSCREEN:
        keymap[i] = SDLK_PRINT;
        break;
      case SCANCODE_BREAK:
        keymap[i] = SDLK_BREAK;
        break;
      case SCANCODE_BREAK_ALTERNATIVE:
        keymap[i] = SDLK_PAUSE;
        break;
      case SCANCODE_LEFTSHIFT:
        keymap[i] = SDLK_LSHIFT;
        break;
      case SCANCODE_RIGHTSHIFT:
        keymap[i] = SDLK_RSHIFT;
        break;
      case SCANCODE_LEFTCONTROL:
        keymap[i] = SDLK_LCTRL;
        break;
      case SCANCODE_RIGHTCONTROL:
        keymap[i] = SDLK_RCTRL;
        break;
      case SCANCODE_RIGHTWIN:
        keymap[i] = SDLK_RSUPER;
        break;
      case SCANCODE_LEFTWIN:
        keymap[i] = SDLK_LSUPER;
        break;
      case SCANCODE_LEFTALT:
        keymap[i] = SDLK_LALT;
        break;
      case SCANCODE_RIGHTALT:
        keymap[i] = SDLK_RALT;
        break;
      case 127:
        keymap[i] = SDLK_MENU;
        break;
      /* this should take care of all standard ascii keys */
      default:
        keymap[i] = KVAL(vga_keymap[0][i]);
        break;
    }
  }
  for (i = 0; i < SDL_arraysize(keymap); ++i) {
    switch (keymap_temp[i]) {
      case K_F1:
        keymap[i] = SDLK_F1;
        break;
      case K_F2:
        keymap[i] = SDLK_F2;
        break;
      case K_F3:
        keymap[i] = SDLK_F3;
        break;
      case K_F4:
        keymap[i] = SDLK_F4;
        break;
      case K_F5:
        keymap[i] = SDLK_F5;
        break;
      case K_F6:
        keymap[i] = SDLK_F6;
        break;
      case K_F7:
        keymap[i] = SDLK_F7;
        break;
      case K_F8:
        keymap[i] = SDLK_F8;
        break;
      case K_F9:
        keymap[i] = SDLK_F9;
        break;
      case K_F10:
        keymap[i] = SDLK_F10;
        break;
      case K_F11:
        keymap[i] = SDLK_F11;
        break;
      case K_F12:
        keymap[i] = SDLK_F12;
        break;

      case K_DOWN:
        keymap[i] = SDLK_DOWN;
        break;
      case K_LEFT:
        keymap[i] = SDLK_LEFT;
        break;
      case K_RIGHT:
        keymap[i] = SDLK_RIGHT;
        break;
      case K_UP:
        keymap[i] = SDLK_UP;
        break;

      case K_P0:
        keymap[i] = SDLK_KP0;
        break;
      case K_P1:
        keymap[i] = SDLK_KP1;
        break;
      case K_P2:
        keymap[i] = SDLK_KP2;
        break;
      case K_P3:
        keymap[i] = SDLK_KP3;
        break;
      case K_P4:
        keymap[i] = SDLK_KP4;
        break;
      case K_P5:
        keymap[i] = SDLK_KP5;
        break;
      case K_P6:
        keymap[i] = SDLK_KP6;
        break;
      case K_P7:
        keymap[i] = SDLK_KP7;
        break;
      case K_P8:
        keymap[i] = SDLK_KP8;
        break;
      case K_P9:
        keymap[i] = SDLK_KP9;
        break;
      case K_PPLUS:
        keymap[i] = SDLK_KP_PLUS;
        break;
      case K_PMINUS:
        keymap[i] = SDLK_KP_MINUS;
        break;
      case K_PSTAR:
        keymap[i] = SDLK_KP_MULTIPLY;
        break;
      case K_PSLASH:
        keymap[i] = SDLK_KP_DIVIDE;
        break;
      case K_PENTER:
        keymap[i] = SDLK_KP_ENTER;
        break;
      case K_PDOT:
        keymap[i] = SDLK_KP_PERIOD;
        break;

      case K_SHIFT:
        if (keymap[i] != SDLK_RSHIFT)
          keymap[i] = SDLK_LSHIFT;
        break;
      case K_SHIFTL:
        keymap[i] = SDLK_LSHIFT;
        break;
      case K_SHIFTR:
        keymap[i] = SDLK_RSHIFT;
        break;
      case K_CTRL:
        if (keymap[i] != SDLK_RCTRL)
          keymap[i] = SDLK_LCTRL;
        break;
      case K_CTRLL:
        keymap[i] = SDLK_LCTRL;
        break;
      case K_CTRLR:
        keymap[i] = SDLK_RCTRL;
        break;
      case K_ALT:
        keymap[i] = SDLK_LALT;
        break;
      case K_ALTGR:
        keymap[i] = SDLK_RALT;
        break;

      case K_INSERT:
        keymap[i] = SDLK_INSERT;
        break;
      case K_REMOVE:
        keymap[i] = SDLK_DELETE;
        break;
      case K_PGUP:
        keymap[i] = SDLK_PAGEUP;
        break;
      case K_PGDN:
        keymap[i] = SDLK_PAGEDOWN;
        break;
      case K_FIND:
        keymap[i] = SDLK_HOME;
        break;
      case K_SELECT:
        keymap[i] = SDLK_END;
        break;

      case K_NUM:
        keymap[i] = SDLK_NUMLOCK;
        break;
      case K_CAPS:
        keymap[i] = SDLK_CAPSLOCK;
        break;

      case K_F13:
        keymap[i] = SDLK_PRINT;
        break;
      case K_HOLD:
        keymap[i] = SDLK_SCROLLOCK;
        break;
      case K_PAUSE:
        keymap[i] = SDLK_PAUSE;
        break;

      case 127:
        keymap[i] = SDLK_BACKSPACE;
        break;

      default:
        break;
    }
  }
}

static SDL_keysym*
TranslateKey(int scancode, SDL_keysym* keysym, SDLMod modstate)
{
  /* Set the keysym information */
  keysym->scancode = scancode;
  keysym->sym = keymap[scancode];
  keysym->mod = KMOD_NONE;

  /* If UNICODE is on, get the UNICODE value for the key */
  keysym->unicode = 0;
  if (SDL_TranslateUNICODE) {
    if (KeybAltLayout && scancode < ALT_LAY_LEN) {
      int map = 0;
      if (modstate & KMOD_SHIFT) {
        map = 1;
      }
      if (utf_alt_lay[0][scancode] > 0xff) { // letters
        if (modstate & KMOD_CAPS) {
          map ^= 1;
        }
      }
      keysym->unicode = utf_alt_lay[map][scancode];
    } else {
      int map;

      map = 0;
      if (modstate & KMOD_SHIFT) {
        map |= (1 << KG_SHIFT);
      }
      if (modstate & KMOD_CTRL) {
        map |= (1 << KG_CTRL);
      }
      if (modstate & KMOD_LALT) {
        map |= (1 << KG_ALT);
      }
      if (modstate & KMOD_RALT) {
        map |= (1 << KG_ALTGR);
      }
      if (KTYP(vga_keymap[map][scancode]) == KT_LETTER) {
        if (modstate & KMOD_CAPS) {
          map ^= (1 << KG_SHIFT);
        }
      }
      if (KTYP(vga_keymap[map][scancode]) == KT_PAD) {
        if (modstate & KMOD_NUM) {
          keysym->unicode = KVAL(vga_keymap[map][scancode]);
        }
      } else {
        keysym->unicode = KVAL(vga_keymap[map][scancode]);
      }
    }
  }
  return (keysym);
}

int
KMSDRM_InitInput(_THIS)
{
#if SDL_INPUT_TSLIB
  ts_dev = NULL;
#endif /* SDL_INPUT_TSLIB */
  mouse_fd = -1;
  keyboard_fd = -1;
  /* Enable mouse and keyboard support */
  if (FB_OpenKeyboard(this) < 0) {
    return (-1);
  }
  if (FB_OpenMouse(this) < 0) {
    const char* sdl_nomouse;

    sdl_nomouse = SDL_getenv("SDL_NOMOUSE");
    if (!sdl_nomouse) {
      SDL_SetError("Unable to open mouse");
      FB_CloseKeyboard(this);
      return (-1);
    }
  }

  return 0;
}

void
KMSDRM_ExitInput(_THIS)
{
  FB_CloseMouse(this);
  FB_CloseKeyboard(this);
}

#endif