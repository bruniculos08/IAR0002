#include "logic.h"
#include <SDL2/SDL.h>

// g++ main.cpp -lSDL2 -o main && ./main

int main(void){
    
    SDL_Window *window;
    window = NULL;

    SDL_Renderer *renderer;
    renderer = NULL;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawPoint(renderer, WINDOW_WIDTH/2, WINDOW_HEIGHT/2);

    SDL_RenderPresent(renderer);
    SDL_Delay(10000);

    return 0;
}