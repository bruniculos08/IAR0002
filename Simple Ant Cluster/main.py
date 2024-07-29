import pygame as pygame
import numpy as np
import sys
import math
import random
import datetime

# Pixel's area for every particle is (block_side)²:
BLOCK_SIDE = 10

BLACK = (0, 0, 0)
WHITE = (200, 200, 200)
RED = (200, 50, 50)
GREEN = (50, 200, 50)
BLUE = (50, 32, 240)
WINDOW_HEIGHT = 55 * BLOCK_SIDE
WINDOW_WIDTH = 55 * BLOCK_SIDE 
SCREEN = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
CLOCK = pygame.time.Clock()
SCREEN.fill(WHITE)
VISION = 4
ALPHA = 50
DATA_NUMBER = 500
ANT_NUMBER = 50
ITERATIONS = 100000
COUNTER = 0
FLAG_COUNTER = False

class Ant:
    def __init__(self, ant_grid):
        self.rectangle = None
        self.carrying_data = None
        self.data_around = 0

        while True:
            self.x = random.randint(0, WINDOW_WIDTH//BLOCK_SIDE - 1)
            self.y = random.randint(0, WINDOW_HEIGHT//BLOCK_SIDE - 1)
            # ant_grid[y][x] == None se não há formiga nesta posição
            # ant_grid[y][x] == self se há formiga nesta posição
            if ant_grid[self.y][self.x] == None:
                ant_grid[self.y][self.x] = self
                self.rectangle = pygame.Rect(self.x * BLOCK_SIDE, self.y * BLOCK_SIDE, BLOCK_SIDE, BLOCK_SIDE)
                break

class Data:
    def __init__(self, data_grid):
        self.rectangle = None
        self.carrier = None
        self.being_carried = False

        while True:
            self.x = random.randint(0, WINDOW_WIDTH//BLOCK_SIDE - 1)
            self.y = random.randint(0, WINDOW_HEIGHT//BLOCK_SIDE - 1)
            # data_grid[y][x] == None se não há um dado nesta posição
            # data_grid[y][x] == self se há um dado nesta posição
            if data_grid[self.y][self.x] == None:
                data_grid[self.y][self.x] = self
                self.rectangle = pygame.Rect(self.x * BLOCK_SIDE, self.y * BLOCK_SIDE, BLOCK_SIDE, BLOCK_SIDE)
                break

def countAroundData(ant, data_grid, vision):
    ant.data_around = 0
    x = ant.rectangle.left // BLOCK_SIDE
    y = ant.rectangle.top // BLOCK_SIDE

    for ray in range(0, vision + 1):
        directions = [(-ray, 0), (0, -ray), (-ray, -ray), (ray, 0), (0, ray), (ray, ray), (ray, -ray), (-ray, ray)]
        for (dx, dy) in directions:
            seen_x = (x + dx) % (WINDOW_WIDTH // BLOCK_SIDE)
            seen_y = (y + dy) % (WINDOW_HEIGHT // BLOCK_SIDE)
            if (data_grid[seen_y][seen_x] != None):
                ant.data_around += 1
    
    return ant.data_around

def countAroundDataForAll(ants, data_grid, vision):
    for ant in ants:
        countAroundData(ant, data_grid, vision)

def updateDataGrid(datas, data_grid):
    for data in datas:
        x = data.rectangle.left // BLOCK_SIDE
        y = data.rectangle.top // BLOCK_SIDE
        if (data_grid[y][x] == None) and (data.being_carried == False):
            data_grid[y][x] = data

def move(ant, ant_grid):
    x = ant.rectangle.left // BLOCK_SIDE 
    y = ant.rectangle.top // BLOCK_SIDE
    while True:
        dx = random.randint(-1, 1)
        dy = random.randint(-1, 1)
        new_x = (x + dx) % (WINDOW_WIDTH//BLOCK_SIDE) 
        new_y = (y + dy) % (WINDOW_HEIGHT//BLOCK_SIDE) 

        if ant_grid[new_y][new_x] == None:
            break
    
    ant_grid[y][x] = None
    ant_grid[new_y][new_x] = ant
    ant.rectangle.left = new_x * BLOCK_SIDE
    ant.rectangle.top = new_y * BLOCK_SIDE
    
    if(ant.carrying_data != None):
        ant.carrying_data.rectangle.left = new_x * BLOCK_SIDE
        ant.carrying_data.rectangle.top = new_y * BLOCK_SIDE

def updateAll(ants, datas, ant_grid, data_grid):
    countAroundDataForAll(ants, data_grid, VISION)
    updateDataGrid(datas, data_grid)
    global COUNTER
    global FLAG_COUNTER
    global ITERATIONS

    COUNTER += 1
    # print("COUNTER = " + str(COUNTER))

    if(COUNTER == 1):
        pygame.image.save(SCREEN, "inicial_grid.jpeg")

    if(COUNTER == ITERATIONS):
        FLAG_COUNTER = True

    to_remove_ants = []

    for ant in ants:
        x = ant.rectangle.left // BLOCK_SIDE 
        y = ant.rectangle.top // BLOCK_SIDE

        # Caso 01 - Formiga sem dado em cima de um dado:
        if (ant.carrying_data == None) and (data_grid[y][x] != None):
            if(FLAG_COUNTER):
                to_remove_ants.append(ant)
            else:
                pick_probability = calcPickProbability(ant, VISION)
                if random.random() < pick_probability:
                    data = data_grid[y][x]
                    data_grid[y][x] = None
                    ant.carrying_data = data
                    data.being_carried = True
                    data.carrier = ant
        # Caso 02 - Formiga com dado em cima de nada:
        elif (ant.carrying_data != None) and (data_grid[y][x] == None):
            drop_probability = calcDropProbability(ant, VISION)
            if random.random() < drop_probability:
                data = ant.carrying_data
                ant.carrying_data = None
                data_grid[y][x] = data
                data.being_carried = False
                data.carrier = None
        # Sempre mover a formiga (e o dado que esta estiver carregado):
        move(ant, ant_grid)

    for ant in to_remove_ants:
        x = ant.rectangle.left // BLOCK_SIDE 
        y = ant.rectangle.top // BLOCK_SIDE

        ant_grid[y][x] = None
        ants.remove(ant)
         
def calcPickProbability(ant, vision):
    return max(0, 1 - ((ant.data_around ** 2)/(vision * ALPHA)))

def calcDropProbability(ant, vision):
    return (ant.data_around ** 2)/(vision * ALPHA)

def DrawGrid(ant_grid, data_grid):
    for y in range(0, WINDOW_HEIGHT//BLOCK_SIDE):
        for x in range(0, WINDOW_WIDTH//BLOCK_SIDE):
            if (ant_grid[y][x] != None) and (ant_grid[y][x].carrying_data != None):
                pygame.draw.rect(SCREEN, BLUE, ant_grid[y][x].rectangle, 0)
            elif (ant_grid[y][x] != None) and (ant_grid[y][x].carrying_data == None):
                pygame.draw.rect(SCREEN, RED, ant_grid[y][x].rectangle, 0)
            elif (ant_grid[y][x] == None) and (data_grid[y][x] != None):
                pygame.draw.rect(SCREEN, GREEN, data_grid[y][x].rectangle, 0)
            else:
                rectangle = pygame.Rect(x * BLOCK_SIDE, y * BLOCK_SIDE, BLOCK_SIDE, BLOCK_SIDE)
                pygame.draw.rect(SCREEN, WHITE, rectangle, 0)

if __name__ == "__main__":
    data_grid = [[None for x in range(0, WINDOW_WIDTH//BLOCK_SIDE)] for y in range(0, WINDOW_HEIGHT//BLOCK_SIDE)]
    datas = [Data(data_grid) for _ in range(0, DATA_NUMBER)]
    ant_grid = [[None for x in range(0, WINDOW_WIDTH//BLOCK_SIDE)] for y in range(0, WINDOW_HEIGHT//BLOCK_SIDE)]
    ants = [Ant(ant_grid) for _ in range(0, ANT_NUMBER)]

    counter = ITERATIONS
    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
        random.seed(str(datetime.datetime.now()))
        DrawGrid(ant_grid, data_grid)
        updateAll(ants, datas, ant_grid, data_grid)
        if(counter % 1000 == 0):
            print(counter)
            pygame.display.update()
        if(len(ants) == 0):
            DrawGrid(ant_grid, data_grid)
            pygame.image.save(SCREEN, "final_grid.jpeg")
            break
        counter -= 1