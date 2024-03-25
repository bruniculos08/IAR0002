#include "logic.h"

// g++ main.cpp -lSDL2 -o main && ./main

int main(void){
    
    // Variáveis necessárias para iniciar a janela:
    SDL_Window *window;
    window = NULL;
    SDL_Renderer *renderer;
    renderer = NULL;

    // Union de evento:
    SDL_Event event;

    // Criando janela:
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);

    // Criar grid de dados:
    ColoredData ***data_grid;
    data_grid = createDataGrid();
    // Criar grid de formigas:
    Ant ***ant_grid;
    ant_grid = createAntGrid();

    set<ColoredData *> all_data;
    generateData(data_grid, all_data);

    set<Ant *> all_ants;
    generateAnts(ant_grid, all_ants);


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
        
        drawGrid(window, renderer, data_grid, ant_grid);

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

ColoredData ***createDataGrid()
{
    cout << "Line 65" << endl;
    // data_grid é um ponteiro para ponteiros que apontam para ponteiros de ColoredData:
    ColoredData ***data_grid;
    // Alocar (floorf(WINDOW_HEIGHT/BLOCK_SIZE)) ponteiros para ponteiros de ColoredData;
    data_grid = (ColoredData ***) malloc(sizeof(ColoredData **) * floorf(WINDOW_HEIGHT/BLOCK_SIZE));
    for (int y = 0; y < floorf(WINDOW_HEIGHT/BLOCK_SIZE); y++)
    {
        // Para cada ponteiro de ponteiros de ColoredData, alocar (floorf(WINDOW_WIDTH/BLOCK_SIZE)) ponteiros de ColoredData:
        *(data_grid + y) = (ColoredData **) malloc(sizeof(ColoredData *) * floorf(WINDOW_WIDTH/BLOCK_SIZE));
        for (int x = 0; x < floorf(WINDOW_WIDTH/BLOCK_SIZE); x++)
        {
            // Colocar o valor de cada ponteiro de ColoredData como NULL:
            *(*(data_grid + y) + x) = NULL;
        }
    }
    // Retorna o valor data_grid, que como explicado inicialmente é o endereço a partir de onde...
    // ... estão alocados em sequência (floorf(WINDOW_HEIGHT/BLOCK_SIZE)) ponteiros para ponteiros de ColoredData:
    cout << "Line 82" << endl;
    return data_grid;
}

Ant ***createAntGrid()
{
    // ant_grid é um ponteiro para ponteiros que apontam para ponteiros de Ant:
    Ant ***ant_grid;
    // Alocar (floorf(WINDOW_HEIGHT/BLOCK_SIZE)) ponteiros para ponteiros de Ant;
    ant_grid = (Ant ***) malloc(sizeof(Ant **) * floorf(WINDOW_HEIGHT/BLOCK_SIZE));
    for (int y = 0; y < floorf(WINDOW_HEIGHT/BLOCK_SIZE); y++)
    {
        // Para cada ponteiro de ponteiros de Ant, alocar (floorf(WINDOW_WIDTH/BLOCK_SIZE)) ponteiros de Ant:
        *(ant_grid + y) = (Ant **) malloc(sizeof(Ant *) * floorf(WINDOW_WIDTH/BLOCK_SIZE));
        for (int x = 0; x < floorf(WINDOW_WIDTH/BLOCK_SIZE); x++)
        {
            // Colocar o valor de cada ponteiro de ColoredData como NULL:
            *(*(ant_grid + y) + x) = NULL;
        }
    }
    // Retorna o valor ant_grid, que como explicado inicialmente é o endereço a partir de onde...
    // ... estão alocados em sequência (floorf(WINDOW_HEIGHT/BLOCK_SIZE)) ponteiros para ponteiros de Ant:
    return ant_grid;
}

ColoredData::ColoredData(ColoredData ***data_grid, ColoredData *ptr)
{
    carrier = NULL;
    do
    {
        // A classe já tem variáveis x e y:
        x = rand() % (WINDOW_WIDTH/BLOCK_SIZE);
        y = rand() % (WINDOW_HEIGHT/BLOCK_SIZE);
    } while (data_grid[y][x] != NULL);
    data_grid[y][x] = ptr;
    
    for (int i = 0; i < 3; i++) color[i] = rand() % 256;
    color[3] = (rand() % (256 - 128)) + 128;
}

Ant::Ant(Ant ***ant_grid, Ant *ptr)
{
    carried = NULL;
    do
    {
        // A classe já tem variáveis x e y:
        x = rand() % (WINDOW_WIDTH/BLOCK_SIZE);
        y = rand() % (WINDOW_HEIGHT/BLOCK_SIZE);
    } while (ant_grid[y][x] != NULL);
    ant_grid[y][x] = ptr;
}

void drawGrid(SDL_Window *window, SDL_Renderer *renderer, ColoredData ***data_grid, Ant ***ant_grid)
{
    for (int y = 0; y < floorf(WINDOW_HEIGHT/BLOCK_SIZE); y++)
    {
        for (int x = 0; x < floorf(WINDOW_WIDTH/BLOCK_SIZE); x++)
        { 
            // (0) Criar bloco atual a ser desenhado:
            SDL_Rect rect{x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
            
            // (1) Caso no bloco não haja formiga mas haja um dado:
            if((ant_grid[y][x] == NULL) && (data_grid[y][x] != NULL))
            {
                ColoredData *current_data;
                current_data = data_grid[y][x];
                int R = (*current_data).color[0];
                int G = (*current_data).color[1];
                int B = (*current_data).color[2];
                int alpha = (*current_data).color[3];
                SDL_SetRenderDrawColor(renderer, R, G, B, alpha);
                SDL_RenderFillRect(renderer, &rect);
            }
            // (2) Caso no bloco haja uma formiga sem carregar um dado:
            else if((ant_grid[y][x] != NULL) && (*ant_grid[y][x]).carried == NULL)
            {
                SDL_SetRenderDrawColor(renderer, 220, 50, 50, 100);
                SDL_RenderFillRect(renderer, &rect);
            } 
            // (3) Caso no bloco haja uma formiga carregando um dado:
            else if((ant_grid[y][x] != NULL) && (*ant_grid[y][x]).carried != NULL)
            {
                SDL_SetRenderDrawColor(renderer, 50, 50, 220, 100);
                SDL_RenderFillRect(renderer, &rect);
            }
            // (4) Caso no bloco não haja nenhuma formiga ou dado:
            else // if((ant_grid[y][x] == NULL) && (data_grid[y][x] == NULL))
            {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}

void generateData(ColoredData ***data_grid, set<ColoredData*> &all_data)
{
    for(int i = 0; i < DATA_NUMBER; i++)
    {
        ColoredData *ptr;
        ptr = (ColoredData *) malloc(sizeof(ColoredData));
        *ptr = ColoredData(data_grid, ptr);
        all_data.insert(ptr);
    }
}

void generateAnts(Ant ***ant_grid, set<Ant*> &all_ants)
{
    for(int i = 0; i < ANTS_NUMBER; i++)
    {
        Ant *ptr;
        ptr = (Ant *) malloc(sizeof(Ant));
        *ptr = Ant(ant_grid, ptr);
        all_ants.insert(ptr);
    }
}