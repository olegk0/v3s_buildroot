#include <SDL/SDL.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

unsigned char
toUTF8(Uint16 codepoint, char utf8[4])
{
  if (codepoint > 0x7FF) {
    utf8[0] = 0xe0 | ((codepoint >> 12) & 0xf);
    utf8[1] = 0x80 | ((codepoint >> 6) & 0x3f);
    utf8[2] = 0x80 | (codepoint & 0x3f);
    utf8[3] = 0;
    return 3;
  } else if (codepoint > 0x7F) {
    utf8[0] = 0xc0 | ((codepoint >> 6) & 0x1f);
    utf8[1] = 0x80 | (codepoint & 0x3f);
    utf8[2] = 0;
    return 2;
  }
  utf8[0] = codepoint & 0x7F;
  utf8[1] = 0;
  return 1;
}

int
log_events()
{
  SDL_Event event;
  if (SDL_PollEvent(
        &event)) {        /* Loop until there are no events left on the queue */
    switch (event.type) { /* Process the appropiate event type */
      case SDL_KEYDOWN:   /* Handle a KEYDOWN event */
      {

        char uc[4];

        toUTF8(event.key.keysym.unicode, uc);
        printf("\nKey press sym:%d  scancode:%d  unicode:%X  %s\n",
               event.key.keysym.sym,
               event.key.keysym.scancode,
               event.key.keysym.unicode, uc);
        /*
                toUTF8(0x006D, uc);
                printf("%s\n", uc);
                toUTF8(0x0416, uc);
                printf("%s\n", uc);
                toUTF8(0x4E3D, uc);
                printf("%s\n", uc);
                */
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          return 0;
        }
      } break;
      case SDL_KEYUP:
        printf("Key up: %d\n", event.key.keysym.sym);
        break;
      case SDL_MOUSEMOTION:
        printf("Mouse motion\n");
        break;
      case SDL_QUIT:
        printf("Quit\n");
        return 0;
      default: /* Report an unhandled event */
        printf("Unknown Event: %d\n", event.type);
        break;
    }
    return event.type;
  }
  return 1;
}

int
main(int argc, char* argv[])
{
  SDL_Init(SDL_INIT_VIDEO);

  SDL_EnableUNICODE(1);
  if (argc > 1) {
    printf(">%c\n", argv[1][0]);
    if (argv[1][0] == 'n') {
    }
  }

  SDL_Surface* screen = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE);

  if (screen == NULL) {
    printf("Could not create window: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Rect rect;
  rect.x = 0;
  rect.y = 0;
  rect.w = screen->w;
  rect.h = screen->h;

  SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, rand() % 255,
  rand() % 255, rand() % 255));

  time_t start = time(NULL);
  while (1) {
    rect.x = rand() % screen->w;
    rect.y = rand() % screen->h;
    rect.w = rand() % (screen->w - rect.x);
    rect.h = rand() % (screen->h - rect.y);

    SDL_FillRect(
      screen,
      &rect,
      SDL_MapRGB(screen->format, rand() % 255, rand() % 255, rand() % 255));
    SDL_Flip(screen);

    int r = 0, cnt = 0;
    do {
      usleep(100000);
      r = log_events();
    } while (r == 1 && cnt++ < 10);

    if (r == 0)
      break;
  }

  SDL_Quit();

  return 0;
}
