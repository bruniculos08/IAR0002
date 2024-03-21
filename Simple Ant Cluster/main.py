import pygame as pygame
import numpy as np
import sys
import math
import random
import datetime

RED = (200, 50, 50)
BLACK = (0, 0, 0)
WHITE = (200, 200, 200)
GREEN = (50, 200, 50)
PURPLE = (50, 32, 240)
# WINDOW_HEIGHT = 1080
WINDOW_HEIGHT = 800
# WINDOW_WIDTH = 1920
WINDOW_WIDTH = 800
SCREEN = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
CLOCK = pygame.time.Clock()
SCREEN.fill(WHITE)
VISION = 4
ALPHA = 200
DATA_NUMBER = 800
ANT_NUMBER = 20

# Pixel's area for every particle is (block_side)²:
BLOCK_SIDE = 10

class Ant:
    def __init__(self, ant_grid):
        self.rectangle = None
        self.carrying_data = None
        self.data_around = 0

        while True:
            self.x = random.randint(0, WINDOW_WIDTH//BLOCK_SIDE - 1)
            self.y = random.randint(0, WINDOW_HEIGHT//BLOCK_SIDE - 1)
            # grid[y][x] == 0 se não há formiga nesta posição
            # grid[y][x] == 1 se há formiga nesta posição
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
            # grid[y][x] == 0 se não há formiga nesta posição
            # grid[y][x] == 1 se há formiga nesta posição
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

    for ant in ants:
        x = ant.rectangle.left // BLOCK_SIDE 
        y = ant.rectangle.top // BLOCK_SIDE

        # Caso 01 - Formiga sem dado em cima de um dado:
        if (ant.carrying_data == None) and (data_grid[y][x] != None):
            pick_probability = calcPickProbability(ant, VISION)
            # print("pick_probability = " + str(pick_probability))
            if random.random() < pick_probability:
                data = data_grid[y][x]
                data_grid[y][x] = None
                ant.carrying_data = data
                data.being_carried = True
                data.carrier = ant
                # print("Formiga pegou um dado")
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
            
def calcPickProbability(ant, vision):
    return max(0, 1 - ((ant.data_around ** 2)/(vision * ALPHA)))

def calcDropProbability(ant, vision):
    return (ant.data_around ** 2)/(vision * ALPHA)

def DrawGrid(ant_grid, data_grid):
    for y in range(0, WINDOW_HEIGHT//BLOCK_SIDE):
        for x in range(0, WINDOW_WIDTH//BLOCK_SIDE):
            if (ant_grid[y][x] != None) and (ant_grid[y][x].carrying_data != None):
                pygame.draw.rect(SCREEN, PURPLE, ant_grid[y][x].rectangle, 0)
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

    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
        random.seed(str(datetime.datetime.now()))
        updateAll(ants, datas, ant_grid, data_grid)
        DrawGrid(ant_grid, data_grid)
        pygame.display.update()