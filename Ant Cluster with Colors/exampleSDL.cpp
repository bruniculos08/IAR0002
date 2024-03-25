#include "logic.h"
#include <SDL2/SDL.h>

// g++ main.cpp -lSDL2 -o main && ./main

int main(void){
    
    // Variáveis necessárias para iniciar a janela:
    SDL_Window *window;
    window = NULL;
    SDL_Renderer *renderer;
    renderer = NULL;

    // Union de evento:
    SDL_Event event;

    // Struct de retângulo:
    SDL_Rect rect{WINDOW_WIDTH/4, WINDOW_HEIGHT/4,  WINDOW_WIDTH/4, WINDOW_HEIGHT/4};

    // Criando janela:
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);


    bool running = true;
    while (running)
    {
        // Verificação de condição para fechar janela:
        while (SDL_PollEvent(&event))
        {
            // Obs.: a função "SDL_PollEvent(SDL_Event *event)" verifica se há um evento na fila de eventos e se houver...
            // ... e "&event == NULL" coloca este evento em "&event" (caso "&event != NULL" não altera o conteúdo em "&event")...
            // ... e além disso, independente de "&event", retorna 1 (true) se houver evento na fila e 0 (false) se não houver.

            // Se o tipo do evento for o botão de fechamento da janela:
            if(event.type == SDL_QUIT) running = false;
        }
        

        // Define a cor atual de desenho para preto (RGB alpha):
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        // Desenha a cor de desenho sobre todo o alvo do renderizador (a janela inteira):
        SDL_RenderClear(renderer);

        // Define a cor atual de desenho para verde (RGB alpha):
        SDL_SetRenderDrawColor(renderer, 100, 240, 100, 255);
        // Desenha/preenche um retângulo:
        SDL_RenderFillRect(renderer, &rect);

        // Define a cor atual de desenho para branco (RGB alpha):
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        // // Desenha um ponto (pixel) nas coordenadas passadas:
        SDL_RenderDrawPoint(renderer, WINDOW_WIDTH/2, WINDOW_HEIGHT/2);

        // Faz update da tela (atualiza os desenhos do renderer):
        SDL_RenderPresent(renderer);
        // Equivalente a função "sleep()":
        SDL_Delay(10);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}