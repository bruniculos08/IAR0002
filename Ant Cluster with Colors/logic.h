#include <bits/stdc++.h>
#include <SDL2/SDL.h>
using namespace std;

// Tamanho dos "pixels" (cada quadrado que representa uma formiga ou um dado):
#define BLOCK_SIZE 10
// Dimensões da janela:
#define WINDOW_HEIGHT 1080
#define WINDOW_WIDTH 1920
// Número de formigas:
#define ANTS_NUMBER 100
// Número de dados:
#define DATA_NUMBER 100

class ColoredData;
class Ant;

// Vamos criar um tipo de dado com 3 dimensões representando uma cor, assim podemos representar no grid...
// ... estes dados de acordo com suas respectivas cores:
class ColoredData {
    public:
        int x, y, color[4];
        // Obs.: as coordenadas x e y são os valores das coordenadas reais após a divisão (floor) por BLOCK_SIZE.
        Ant *carrier;
        // O método construtor da classe deve receber uma matriz de endereços para objetos da classe:
        ColoredData(ColoredData ***data_grid, ColoredData *ptr);
};

class Ant {
    public:
        int x, y;
        // Obs.: as coordenadas x e y são os valores das coordenadas reais após a divisão (floor) por BLOCK_SIZE.
        ColoredData *carried;
        Ant(Ant ***ant_grid, Ant *ptr);
};

ColoredData ***createDataGrid();

Ant ***createAntGrid();

void drawGrid(SDL_Window *window, SDL_Renderer *renderer, ColoredData ***data_grid, Ant ***ant_grid);

void generateData(ColoredData ***data_grid, set<ColoredData*> &all_data);

void generateAnts(Ant ***ant_grid, set<Ant*> &all_ants);

