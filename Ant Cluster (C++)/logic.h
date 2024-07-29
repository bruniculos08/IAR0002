#include <bits/stdc++.h>
#include <SDL2/SDL.h>
using namespace std;

// Tamanho dos "pixels" (cada quadrado que representa uma formiga ou um dado):
#define BLOCK_SIZE 10
// Dimensões da janela:
#define WINDOW_HEIGHT 55 * BLOCK_SIZE
#define WINDOW_WIDTH 55 * BLOCK_SIZE
// Número de formigas:
#define ANTS_NUMBER 10
// Número de dados:  (0.165847 0.166234) (0.166457 0.150838 0.135105) (0.438158) (0.214194 0.204474 0.242168)
#define DATA_NUMBER 600
// Raio de visão das formigas:
#define RADIUS 2
// Fator de escala para dissimilaridade (será variável global pois se definirá futuramente por um função):
// int ALPHA = 13;
#define ALPHA 5.0L
// Delay:
#define DELAY 50
// Iterações:
int ITERATIONS = 2000000;
// Constantes
#define K1 0.8L
#define K2 0.4L
// Ptake = (k1/(k1 + f_i))²
// Pdrop = (f_i/(k2 + f_i))²

#define MY_PI  3.14159265358979323846L
// Intervalos para cálculo de integral:
#define INTERVALS 10
// Precisão para resolução de equação de probabilidade comulativa P(X) = r:
#define PRECISION 0.1L
// Deslocamento para o início não continuo de funções:
#define EPSILON 0.00001L
// Maior número aleatório possível para o gerador deste código:
#define MY_RAND_MAX 100.0L
// Parâmetro de deslocamento da distribuição de Levy:
#define U_LEVY 1.0L
// Parâmetro de escala da distribuição de Levy:
#define C_LEVY 0.4L

#define UPDATE_RATE 10000


int group_4colors[4][3] = {{255, 165, 0}, {128, 0, 128}, {0, 128, 0}, {0, 0, 255}};
int group_15colors[15][3] = {
    {255, 0, 0},       // Red
    {0, 255, 0},       // Green
    {0, 0, 255},       // Blue
    {255, 255, 0},     // Yellow
    {255, 0, 255},     // Magenta
    {0, 255, 255},     // Cyan
    {128, 0, 128},     // Purple
    {255, 165, 0},     // Orange
    {0, 128, 0},       // Dark Green
    {128, 128, 0},     // Olive
    {0, 128, 128},     // Teal
    {165, 42, 42},     // Brown
    {128, 0, 0},       // Maroon
    {128, 128, 128},   // Gray
    {0, 0, 128}        // Navy
};
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

void cleanAnts(Data ***data_grid, Ant ***ant_grid, set<Ant*> &all_ants);

void forceCleanAnts(Data ***data_grid, Ant ***ant_grid, set<Ant*> &all_ants);

void generateResults(int n, string file_path, string data_path, int group_colors[][3], int dim, int num_groups, bool force_flag);

long double calcAverageDistGroup(Data ***data_grid, set<Data*> &all_data);

void freeGrids(Data ***data_grid, set<Data*> &all_data, Ant ***ant_grid, set<Ant*> &all_ants);

void freeMem(set<Data*> &all_data, set<Ant*> &all_ants, Ant ***ant_grid, Data ***data_grid);

// Gerador de número aleatório baseado na distribuição de Levy
// ideia vista em: https://www.scirp.org/journal/paperinformation?paperid=96275 para geração de números aleatórios...
// ... baseados em quaisquer distribuição a partir de um gerador aleatório uniforme:

long double randomLevyDistribution();

long double levyDistribution(long double x);

long double solveComulative(long double (* probabilityFunction)(long double x), long double r, long double start_x, long double end_x);

long double solveIntegral(long double (* function)(long double x), long double a, long double b, int intervals);