#include <bits/stdc++.h>
#include <SDL2/SDL.h>
using namespace std;

// Tamanho dos "pixels" (cada quadrado que representa uma formiga ou um dado):
#define BLOCK_SIZE 10
// Dimensões da janela:
#define WINDOW_HEIGHT 50 * BLOCK_SIZE
#define WINDOW_WIDTH 50 * BLOCK_SIZE
// Número de formigas:
#define ANTS_NUMBER 20
// Número de dados:  (0.165847 0.166234) (0.166457 0.150838 0.135105) (0.438158) (0.214194 0.204474 0.242168)
#define DATA_NUMBER 600
// Raio de visão das formigas:
#define RADIUS 2
// Fator de escala para dissimilaridade (será variável global pois se definirá futuramente por um função):
// int ALPHA = 13;
#define ALPHA 0.2
// Delay:
#define DELAY 1000
// Iterações:
int ITERATIONS = 7000000;
// Constantes
#define K1 0.05
#define K2 0.95
// Ptake = (k1/(k1 + f_i))²
// Pdrop = (f_i/(k2 + f_i))²

int group_colors[4][3] = {{10, 200, 40}, {150, 220, 200}, {20, 250, 20}, {20, 250, 200}};

// A fazer: gerar arquivos de sequencia de probabilidades (take e drop) com diferentes parâmetros.

class Data;
class Ant;

// Vamos criar um tipo de dado com 3 dimensões representando uma cor, assim podemos representar no grid...
// ... estes dados de acordo com suas respectivas cores:
class Data {
    public:
        int x, y, dim, group;
        long double *attributes;
        int *color;

        // Obs.: as coordenadas x e y são os valores das coordenadas reais após a divisão (floor) por BLOCK_SIZE.
        Ant *carrier;
        // O método construtor da classe deve receber uma matriz de endereços para objetos da classe:
        Data(Data ***data_grid, Data *ptr, int dim);
};

class Ant {
    public:
        int x, y;
        // Obs.: as coordenadas x e y são os valores das coordenadas reais após a divisão (floor) por BLOCK_SIZE.
        Data *carried;
        Ant(Ant ***ant_grid, Ant *ptr);
};

Data ***createDataGrid();

Ant ***createAntGrid();

long double euclideanDistance(long double *atrr0, long double *atrr1, int size);

void drawGrid(SDL_Window *window, SDL_Renderer *renderer, Data ***data_grid, Ant ***ant_grid);

void generateData(Data ***data_grid, set<Data*> &all_data);

void generateAnts(Ant ***ant_grid, set<Ant*> &all_ants);

long double DRand(long double fMin, long double fMax);

int clampi(int x, int low, int high);

void generateDataFromFile(Data ***data_grid, set<Data*> &all_data, string file_path, int atrr_num, int *group_colors[3], int num_groups);

long double similarity(int x_coord, int y_coord, Data ***data_grid, Ant *current, Ant ***ant_grid);

long double takeProbability(int x_coord, int y_coord, Data ***data_grid, Data *current, Ant ***ant_grid);

long double dropProbability(int x_coord, int y_coord, Data ***data_grid, Data *current, Ant ***ant_grid);

void move(Data ***data_grid, Ant ***ant_grid, Ant *current);

void updateAll(Data ***data_grid, set<Data*> &all_data, Ant ***ant_grid, set<Ant*> &all_ants);

int positiveModulo(int i, int n);

void saveScreen(SDL_Renderer *renderer, string name);

void normalizeAttr(set<Data*> &all_data);

void normalizeDataSet(set<Data*> &all_data);

void cleanAnts(Ant ***ant_grid, set<Ant*> &all_ants);

void generateResults(int n, string file_path, string data_path, int *group_colors[3], int dim, int num_groups);

long double calcAverageDistGroup(Data ***data_grid, set<Data*> &all_data);

void freeGrids(Data ***data_grid, set<Data*> &all_data, Ant ***ant_grid, set<Ant*> &all_ants);

void freeMem(set<Data*> &all_data, set<Ant*> &all_ants, Ant ***ant_grid, Data ***data_grid);