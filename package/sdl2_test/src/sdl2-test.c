#include <SDL2/SDL.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

int log_events(){
    SDL_Event event;
    if(SDL_PollEvent(&event)){  /* Loop until there are no events left on the queue */
        switch(event.type){  /* Process the appropiate event type */
            case SDL_KEYDOWN:  /* Handle a KEYDOWN event */
                printf("Key down %d\n", event.key.keysym.sym);
                if(event.key.keysym.sym == SDLK_ESCAPE){
                    return 0;
                }
                break;
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

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    if(argc > 1){
	printf(">%c\n", argv[1][0]);
	if(argv[1][0] == 'n'){
	}
    }

    SDL_Window * win = SDL_CreateWindow("Hello World!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);// SDL_WINDOW_RESIZABLE

    if (win == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, 0);
    if (ren == NULL) {
        printf("Could not create renderer: %s\n", SDL_GetError());
        return 1;
    }

    time_t start = time(NULL);

    while(1){
        SDL_SetRenderDrawColor(ren, rand() % 255, rand() % 255, rand() % 255, 255);
	SDL_RenderFillRect(ren, NULL);
        SDL_RenderPresent(ren);

	int r = 0, cnt = 0;
	do{
	    usleep(100000);
	    r = log_events();
	}while(r==1 && cnt++ < 10);


	if(r == 0)
	    break;
    }

    SDL_Quit();

    return 0;
}
