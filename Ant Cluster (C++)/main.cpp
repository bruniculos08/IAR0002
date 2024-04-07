#include "logic.h"

/* 
g++ main.cpp -lSDL2 -o main && ./main
    ou
bash build.sh main.cpp 
*/

int main(void){
    
    srand(time(0));

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
    Data ***data_grid;
    data_grid = createDataGrid();
    // Criar grid de formigas:
    Ant ***ant_grid;
    ant_grid = createAntGrid();

    set<Data *> all_data;

    set<Ant *> all_ants;
    generateAnts(ant_grid, all_ants);

    // generateDataFromFile(data_grid, all_data, "data/FifteenSets.txt", 2, NULL, 15);
    generateDataFromFile(data_grid, all_data, "data/FifteenSets.txt", 2, NULL, 15);

    normalizeDataSet(all_data);

    bool running = true;
    int counter = ITERATIONS;
    while (running && !(all_ants.empty()))
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

        updateAll(data_grid, all_data, ant_grid, all_ants);
        
        if(counter == ITERATIONS)
        {
            drawGrid(window, renderer, data_grid, ant_grid);
            SDL_RenderPresent(renderer);
            saveScreen(renderer, "images/initial");
        }
        else if(counter == 0)
        {
            cleanAnts(ant_grid, all_ants);
        }

        if(all_ants.empty())
        {
            drawGrid(window, renderer, data_grid, ant_grid);
            SDL_RenderPresent(renderer);
            saveScreen(renderer, "images/final");
            SDL_Delay(DELAY);
            cout << "Result = " << calcAverageDistGroup(data_grid, all_data) << endl;
        }

        // Tirar comentário seguinte para janela atualizar a cada iteração:
        // drawGrid(window, renderer, data_grid, ant_grid);
        // SDL_RenderPresent(renderer);
        // SDL_Delay(DELAY);
        
        // Faz update da tela (atualiza os desenhos do renderer):
        // Equivalente a função "sleep()":
        // SDL_Delay(DELAY);
        if(counter > 0) counter--;
        // cout << counter << endl;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

Data ***createDataGrid()
{
    // data_grid é um ponteiro para ponteiros que apontam para ponteiros de Data:
    Data ***data_grid;
    // Alocar (floorf(WINDOW_HEIGHT/BLOCK_SIZE)) ponteiros para ponteiros de Data;
    data_grid = (Data ***) malloc(sizeof(Data **) * floorf(WINDOW_HEIGHT/BLOCK_SIZE));
    for (int y = 0; y < floorf(WINDOW_HEIGHT/BLOCK_SIZE); y++)
    {
        // Para cada ponteiro de ponteiros de Data, alocar (floorf(WINDOW_WIDTH/BLOCK_SIZE)) ponteiros de Data:
        *(data_grid + y) = (Data **) malloc(sizeof(Data *) * floorf(WINDOW_WIDTH/BLOCK_SIZE));
        for (int x = 0; x < floorf(WINDOW_WIDTH/BLOCK_SIZE); x++)
        {
            // Colocar o valor de cada ponteiro de Data como NULL:
            *(*(data_grid + y) + x) = NULL;
        }
    }
    // Retorna o valor data_grid, que como explicado inicialmente é o endereço a partir de onde...
    // ... estão alocados em sequência (floorf(WINDOW_HEIGHT/BLOCK_SIZE)) ponteiros para ponteiros de Data:
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
            // Colocar o valor de cada ponteiro de Data como NULL:
            *(*(ant_grid + y) + x) = NULL;
        }
    }
    // Retorna o valor ant_grid, que como explicado inicialmente é o endereço a partir de onde...
    // ... estão alocados em sequência (floorf(WINDOW_HEIGHT/BLOCK_SIZE)) ponteiros para ponteiros de Ant:
    return ant_grid;
}

Data::Data(Data ***data_grid, Data *ptr, int dim)
{
    this->carrier = NULL;
    do
    {
        // A classe já tem variáveis x e y:
        this->x = rand() % (WINDOW_WIDTH/BLOCK_SIZE);
        this->y = rand() % (WINDOW_HEIGHT/BLOCK_SIZE);
    } while (data_grid[y][x] != NULL);
    data_grid[y][x] = ptr;
    
    this->attributes = (long double *) malloc(sizeof(long double) * dim);
    this->color = NULL;
    this->dim = dim;
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
    this->x = x;
    this->y = y;
}

long double euclideanDistance(long double *attr0, long double *attr1, int dim)
{
    long double sum = 0;
    for(int i = 0; i < dim; i++)
    {
        sum += (attr0[i] - attr1[i]) * (attr0[i] - attr1[i]);
    }
    return sqrt(sum);
}

void drawGrid(SDL_Window *window, SDL_Renderer *renderer, Data ***data_grid, Ant ***ant_grid)
{
    // Define a cor atual de desenho para preto (RGB alpha):
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    // Desenha a cor de desenho sobre todo o alvo do renderizador (a janela inteira):
    SDL_RenderClear(renderer);

    for (int y = 0; y < floorf(WINDOW_HEIGHT/BLOCK_SIZE); y++)
    {
        for (int x = 0; x < floorf(WINDOW_WIDTH/BLOCK_SIZE); x++)
        { 
            // (0) Criar bloco atual a ser desenhado:
            SDL_Rect rect{x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
            
            // (1) Caso no bloco não haja formiga mas haja um dado:
            if((ant_grid[y][x] == NULL) && (data_grid[y][x] != NULL))
            {
                Data *current_data;
                current_data = data_grid[y][x];
                if (current_data->color != NULL)
                {
                    int R = (*current_data).color[0];
                    int G = (*current_data).color[1];
                    int B = (*current_data).color[2];
                    SDL_SetRenderDrawColor(renderer, R, G, B, 255);
                }
                else SDL_SetRenderDrawColor(renderer, 50, 220, 50, 255);
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

void generateData(Data ***data_grid, set<Data*> &all_data, int dim)
{
    for(int i = 0; i < DATA_NUMBER; i++)
    {
        Data *ptr;
        ptr = (Data *) malloc(sizeof(Data));
        *ptr = Data(data_grid, ptr, dim);
        (*ptr).attributes[0] = DRand(0, 10000);
        (*ptr).attributes[1] = DRand(0, 10000);
        (*ptr).attributes[2] = DRand(0, 10000);
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

long double DRand(long double fMin, long double fMax)
{
    long double f = (long double)rand() / (long double) RAND_MAX;
    return fMin + f * (fMax - fMin);
}

void printAllDataAttr(set<Data*> &all_data, int dim)
{
    for(Data *x : all_data)
    {
        for (int i = 0; i < dim; i++)
        {
            cout << x->attributes[i] << " ; ";
        }
        cout << endl;
    }
}

void generateDataFromFile(Data ***data_grid, set<Data*> &all_data, string file_path, int dim, int *group_colors[3], int num_groups)
{
    string line, temp;
    int end_index, group;
    char *cstr = new char[1024];
    ifstream my_file (file_path);

    int color_delta = 255/num_groups;

    int color_auto[num_groups][3];
    for (int i = 0; i < num_groups; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            color_auto[i][j] = rand()%256;
        }
    }
    

    if (my_file.is_open())
    {
        while(getline(my_file, line))
        {
            replace(line.begin(), line.end(), ',', '.'); // substituir todo ',' por '.'

            // Se a linha for um comentário:
            if(line[0] == '#' || line[0] == ' ') continue;
            else
            {
                Data *to_add;
                to_add = (Data *) malloc(sizeof(Data));
                (*to_add) = Data(data_grid, to_add, dim);

                for (int i = 0; i < dim; i++)
                {
                    end_index = line.find('\t');
                    temp = line.substr(0, end_index);
                    line = line.substr(end_index + 1, line.length() - end_index);
                    // Converte a string em char*:
                    strcpy(cstr, temp.c_str());
                    sscanf(cstr, "%Lf\t", &to_add->attributes[i]);
                }

                strcpy(cstr, line.c_str());
                sscanf(cstr, "%d\n", &group);
                to_add->group = group;

                // Usar 'group' para determinar a cor:
                if(group_colors != NULL)
                {
                    to_add->color = (int *) malloc(sizeof(int) * 3);
                    to_add->color[0] = group_colors[group - 1][0] % 256;
                    to_add->color[1] = group_colors[group - 1][1] % 256;
                    to_add->color[2] = group_colors[group - 1][2] % 256;
                }
                else
                {
                    to_add->color = (int *) malloc(sizeof(int) * 3);
                    to_add->color[0] = color_auto[group][0];
                    // to_add->color[1] = color_delta * group;
                    to_add->color[1] = color_auto[group][1];
                    to_add->color[2] = color_auto[group][2];
                }

                all_data.insert(to_add);
            }
        }
    }
    my_file.close();
}

int clampi(int x, int low, int high)
{
    assert(low < high);
    if(x < low) return low;
    if(x > high) return high;
    return x;
}

long double similarity(int x_coord, int y_coord, Data ***data_grid, Data *current, Ant ***ant_grid)
{
    int x_coord_neighbor, y_coord_neighbor;
    long double sum = 0.0, s = (2 * RADIUS + 1);

    Data *neighbor;
    pair<int, int> directions[] = {make_pair(-1, 0), make_pair(-1, -1), make_pair(-1, 1),
        make_pair(0, -1), make_pair(0, 1), make_pair(1, 0), make_pair(1, -1), make_pair(1, 1)};

    for (int r = 1; r <= RADIUS; r++)
    {
        for(pair<int, int> &delta : directions)
        {
            x_coord_neighbor = positiveModulo((x_coord + delta.first * r), (WINDOW_WIDTH/BLOCK_SIZE));
            y_coord_neighbor = positiveModulo((y_coord + delta.second * r), (WINDOW_HEIGHT/BLOCK_SIZE));
            if(data_grid[y_coord_neighbor][x_coord_neighbor] != NULL)
            {
                neighbor = data_grid[y_coord_neighbor][x_coord_neighbor];
                sum += (1.0 - euclideanDistance(current->attributes, neighbor->attributes, current->dim) / (long double) ALPHA);
            }
            if(ant_grid[y_coord_neighbor][x_coord_neighbor] != NULL && ant_grid[y_coord_neighbor][x_coord_neighbor]->carried != NULL)
            {
                neighbor = ant_grid[y_coord_neighbor][x_coord_neighbor]->carried;
                sum += (1.0 - euclideanDistance(current->attributes, neighbor->attributes, current->dim) / (long double) ALPHA);

            }
        }
    }
    return sum > 0 ? sum/(s * s - 1) : 0;
}

long double takeProbability(int x_coord, int y_coord, Data ***data_grid, Data *current, Ant ***ant_grid)
{
    long double k1 = K1, f_i = similarity(x_coord, y_coord, data_grid, current, ant_grid);
    return (k1 / (k1 + f_i)) * (k1 / (k1 + f_i));
}

long double dropProbability(int x_coord, int y_coord, Data ***data_grid, Data *current, Ant ***ant_grid)
{
    long double k2 = K2, f_i = similarity(x_coord, y_coord, data_grid, current, ant_grid);
    // Retorno proposto no slide do professor:
    // return (f_i / (k2 + f_i)) * (f_i / (k2 + f_i));
    // Retorno proposto no vídeo que eu encontrei (https://www.youtube.com/watch?v=0lcmeWoOoCU&t=6s&ab_channel=JacovanNiekerk):
    return (f_i < k2) ? (2 * f_i) : 1;
}

void move(Data ***data_grid, Ant ***ant_grid, Ant *current)
{
    int delta_x = (rand() - RAND_MAX/2) % 2;
    int delta_y = (rand() - RAND_MAX/2) % 2;
    
    while(ant_grid[positiveModulo((current->y + delta_y), (WINDOW_HEIGHT/BLOCK_SIZE))][positiveModulo((current->x + delta_x), (WINDOW_WIDTH/BLOCK_SIZE))] != NULL 
        && (delta_x != 0 && delta_y != 0))
    {
        delta_x = (rand() - RAND_MAX/2) % 2;
        delta_y = (rand() - RAND_MAX/2) % 2;
    }
    
    ant_grid[current->y][current->x] = NULL;
    current->x = positiveModulo((current->x + delta_x), (WINDOW_WIDTH/BLOCK_SIZE));
    current->y = positiveModulo((current->y + delta_y), (WINDOW_HEIGHT/BLOCK_SIZE));
    ant_grid[current->y][current->x] = current;

    if(current->carried != NULL)
    {
        current->carried->x = current->x;
        current->carried->y = current->y;
    }
}

void updateAll(Data ***data_grid, set<Data*> &all_data, Ant ***ant_grid, set<Ant*> &all_ants)
{
    for(Ant *current : all_ants)
    {
        int x_coord = current->x;
        int y_coord = current->y;
        long double prob;

        if(current->carried == NULL && data_grid[y_coord][x_coord] != NULL)
        {
            prob = takeProbability(x_coord, y_coord, data_grid, data_grid[y_coord][x_coord], ant_grid);
            if(DRand(0.0, 1.0) <= prob)
            {
                current->carried = data_grid[y_coord][x_coord];
                data_grid[y_coord][x_coord] = NULL;
            }
        }
        else if(current->carried != NULL && data_grid[y_coord][x_coord] == NULL)
        {
            prob = dropProbability(x_coord, y_coord, data_grid, current->carried, ant_grid);
            if(DRand(0.0, 1.0) <= prob)
            {
                data_grid[y_coord][x_coord] = current->carried;
                current->carried = NULL;
            }
        }
        move(data_grid, ant_grid, current);
    }
}

int positiveModulo(int i, int n) {
    return (i % n + n) % n;
}

void saveScreen(SDL_Renderer *renderer, string name)
{
    char *name_bmp = new char[strlen(name.c_str()) + 4 + 1];
    strcpy(name_bmp, name.c_str());
    strcat(name_bmp, ".bmp");
    SDL_Surface *sshot = SDL_CreateRGBSurface(0, WINDOW_WIDTH, WINDOW_HEIGHT, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
    SDL_SaveBMP(sshot, name_bmp);
    SDL_FreeSurface(sshot);
}

void normalizeAttr(set<Data*> &all_data)
{
    long double vector_len;
    for (Data *current : all_data)
    {
        vector_len = 0;
        for (int i = 0; i < current->dim; i++)
        {
            vector_len += current->attributes[i] * current->attributes[i];
        }
        vector_len = sqrt(vector_len);
        for (int i = 0; i < current->dim; i++)
        {
            current->attributes[i] /= vector_len;
        }
    }
}

void normalizeDataSet(set<Data*> &all_data)
{
    assert(!all_data.empty());

    Data *first = *next(all_data.begin(), 0);
    int dim = first->dim;
    long double max_vector[dim], min_vector[dim];
    
    for(int i = 0; i < dim; i++)
    {
        max_vector[i] = first->attributes[i];
        min_vector[i] = first->attributes[i];
    }

    for (Data *current : all_data)
    {
        for (int i = 0; i < dim; i++)
        {
            if(current->attributes[i] > max_vector[i]) max_vector[i] = current->attributes[i];
            if(current->attributes[i] < min_vector[i]) min_vector[i] = current->attributes[i];
        }
    }

    for (Data *current : all_data)
    {
        for (int i = 0; i < dim; i++)
        {
            current->attributes[i] = (current->attributes[i] - min_vector[i]) / (max_vector[i] - min_vector[i]);
            // cout << current->attributes[i] << " ; ";
        }
        // cout << endl;
    }
}

void cleanAnts(Ant ***ant_grid, set<Ant*> &all_ants)
{
    set<Ant*> temp;
    temp = all_ants;
    for (Ant *current : temp)
    {
        if(current->carried == NULL)
        {
            ant_grid[current->y][current->x] = NULL;
            all_ants.erase(current);
            free(current);
        }
    }
    cout << "Number of ants: " << all_ants.size() << endl;
}

void generateResults(int n, string file_path, string data_path, int *group_colors[3], int dim, int num_groups)
{
    file_path.append(".txt");
    ofstream my_file;
    my_file.open(file_path, ios_base::app);
    my_file << "# ALPHA = " << ALPHA << ", RADIUS = " << RADIUS << ", ANTS_NUMBER = " << ANTS_NUMBER << ", K1 = " << K1 << ", K2 = " << K2 << endl;

    srand(time(0));

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
    Data ***data_grid;
    data_grid = createDataGrid();
    // Criar grid de formigas:
    Ant ***ant_grid;
    ant_grid = createAntGrid();

    set<Data *> all_data;
    set<Ant *> all_ants;

    bool running = true;
    for (int i = 0; i < n; i++)
    {
        generateAnts(ant_grid, all_ants);
        generateDataFromFile(data_grid, all_data, data_path, dim, group_colors, num_groups);
        normalizeDataSet(all_data);

        int counter = ITERATIONS;
        while (running && !(all_ants.empty()))
        {
            while (SDL_PollEvent(&event)) if(event.type == SDL_QUIT) running = false;

            updateAll(data_grid, all_data, ant_grid, all_ants);

            if(counter == ITERATIONS)
            {
                drawGrid(window, renderer, data_grid, ant_grid);
                SDL_RenderPresent(renderer);
                saveScreen(renderer, "images/initial");
            }
            else if(counter == 0) cleanAnts(ant_grid, all_ants);
            
            if(all_ants.empty())
            {
                drawGrid(window, renderer, data_grid, ant_grid);
                SDL_RenderPresent(renderer);
                saveScreen(renderer, "images/final");
                SDL_Delay(DELAY);
                freeGrids(data_grid, all_data, ant_grid, all_ants);
            }
            if(counter > 0) counter--;
        }
        my_file << calcAverageDistGroup(data_grid, all_data) << endl;
        freeGrids(data_grid, all_data, ant_grid, all_ants);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

long double calcAverageDistGroup(Data ***data_grid, set<Data*> &all_data)
{
    long double result = 0.0;
    int num_dist = 0;
    double delta_x, delta_y;

    for (Data *current : all_data)
    {
        for (Data *brother : all_data)
        {
            if(brother->group == current->group)
            {
                if(current->x <= brother->x)
                    delta_x = (double) min(brother->x - current->x, current->x - (brother->x - WINDOW_WIDTH/BLOCK_SIZE));
                else
                    delta_x = (double) min(current->x - brother->x, brother->x - (current->x - WINDOW_WIDTH/BLOCK_SIZE));

                if(current->y <= brother->y)
                    delta_y = (double) min(brother->y - current->y, current->y - (brother->y - WINDOW_HEIGHT/BLOCK_SIZE));
                else
                    delta_y = (double) min(current->y - brother->y, brother->y - (current->y - WINDOW_HEIGHT/BLOCK_SIZE));    
                    
                // Como cada distância é adicionada duas vezes, se adiciona apenas...
                // ... metada cada vez:
                result += sqrt(delta_x * delta_x + delta_y * delta_y)/2;
                num_dist ++;
            }
        }
    }
    num_dist /= 2;
    long double w = WINDOW_WIDTH/BLOCK_SIZE;
    long double h = WINDOW_HEIGHT/BLOCK_SIZE;
    long double max_dist = sqrt(h * h + w * w)/2;
    return result / (max_dist * (long double) num_dist);
}

void freeGrids(Data ***data_grid, set<Data*> &all_data, Ant ***ant_grid, set<Ant*> &all_ants)
{
    for(Ant *current : all_ants)
    {
        if(current->carried != NULL)
        {
            free(current->carried);
            all_data.erase(current->carried);
        }
        ant_grid[current->y][current->x] = NULL;
        free(current);
    }
    all_ants.clear();

    for(Data *current : all_data)
    {
        data_grid[current->y][current->x] = NULL;
        free(current->attributes);
        free(current->color);
        free(current);
    }
    all_data.clear();
}