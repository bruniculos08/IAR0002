import pygame as pygame
import numpy as np
import sys
import math
import random
import datetime

BLACK = (0, 0, 0)
WHITE = (200, 200, 200)
GREEN = (50, 200, 50)
PURPLE = (150, 100, 150)
RED = (200, 50, 50)
WINDOW_HEIGHT = 1080
WINDOW_WIDTH = 1920
SCREEN = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
CLOCK = pygame.time.Clock()
SCREEN.fill(WHITE)
ALPHA = 0

# Pixel's area for every particle is (block_side)²:
BLOCK_SIDE = 10

class Grain:
    def __init__(self, data_size, max_data_value):
        self.rectangle = None
        self.similarity = 0
        self.data = []
        for i in range(0, data_size):
            x_i = random.uniform(0, 1) * max_data_value
            self.data.append(x_i)
class Ant:
    def __init__(self):
        self.rectangle = None
        self.carring_grain = None
        self.around_grains = []
        
class Cel:
    def __init__(self):
        self.actual_ant = None
        self.actual_grain = None

def setGrains(cels, data_size, max_data_value):
    # Number of seed particles (stuff the ant will take or not):
    max_grains_number = 100000
    # Actual number of seed particles (max_seeds_number >= actual_seeds_number):
    actual_grains_number = 0
    # List of grains:
    grains = []
    # Probability of generating a grain for each cordinate (x, y):
    probability = 0.05

    for x in range(0, WINDOW_WIDTH, BLOCK_SIDE):
        for y in range(0, WINDOW_HEIGHT, BLOCK_SIDE):
            if(random.random() < probability and actual_grains_number < max_grains_number):
                generated_grain = Grain(data_size, max_data_value)
                generated_grain.rectangle = pygame.Rect(x, y, BLOCK_SIDE, BLOCK_SIDE)
                grains.append(generated_grain)
                cels[y//BLOCK_SIDE][x//BLOCK_SIDE].actual_grain = generated_grain
                actual_grains_number += 1

    return grains

def setAnts(population, cels):
    # Let's suppose that 2 ants can be at the same cordinate (x, y) at the same time:
    ants = []

    for count in range(0, population):
        x = random.randint(0, WINDOW_WIDTH//BLOCK_SIDE - 1) * BLOCK_SIDE
        y = random.randint(0, WINDOW_HEIGHT//BLOCK_SIDE - 1) * BLOCK_SIDE

        if(cels[y//BLOCK_SIDE][x//BLOCK_SIDE].actual_ant == None):
            generated_ant = Ant()
            generated_ant.rectangle = pygame.Rect(x, y, BLOCK_SIDE, BLOCK_SIDE)
            ants.append(generated_ant)
            cels[y//BLOCK_SIDE][x//BLOCK_SIDE].actual_ant = generated_ant
        else:
            count -= 1

    return ants

def drawGridStuff(cels):
    for y in range(0, len(cels)):
        for x in range(0, len(cels[0])):
            # Case 01 - The actual cel has nothing on it:
            if(cels[y][x].actual_ant == None) and (cels[y][x].actual_grain == None):
                rectangle = pygame.Rect(x * BLOCK_SIDE, y * BLOCK_SIDE, BLOCK_SIDE, BLOCK_SIDE)
                pygame.draw.rect(SCREEN, WHITE, rectangle, 0)
            # Case 02 - The actual cel has an ant without grain on it:
            elif(cels[y][x].actual_ant != None) and (cels[y][x].actual_ant.carring_grain == None):
                rectangle = cels[y][x].actual_ant.rectangle
                pygame.draw.rect(SCREEN, RED, rectangle, 0)
            # Case 03 - The actual cel has an ant with grain on it:
            elif(cels[y][x].actual_ant != None) and (cels[y][x].actual_ant.carring_grain != None):
                rectangle = cels[y][x].actual_ant.rectangle
                pygame.draw.rect(SCREEN, PURPLE, rectangle, 0)
            # Case 04 - The actual cel has no ant but has a grain on it:
            elif(cels[y][x].actual_ant == None) and (cels[y][x].actual_grain != None):
                rectangle = cels[y][x].actual_grain.rectangle
                pygame.draw.rect(SCREEN, GREEN, rectangle, 0)

def grainEuclideanDistance(d0, d1):
    sum_squares_diff = sum((d0.data[i] - d1.data[i]) ** 2 for i in range(0, len(d0.data)))
    return math.sqrt(sum_squares_diff)

def averageDistance(grains):
    avg = 0
    count = 0
    for i in range(0, len(grains)):
        for j in range(i + 1, len(grains)):
            count += 1
            avg += grainEuclideanDistance(grains[i], grains[j])
    avg /= count
    return avg

def similarity(i, grains, ant):
    s = len(ant.around_grains) + 1
    f_i = (1/(s ** 2)) * (sum(1 - (grainEuclideanDistance(grains[i], ant.around_grains[j])/ALPHA) 
                            for j in range(0, len(ant.around_grains)) if j != i))
    return max(0, f_i)

# def similarityVector(grains):
#     return [similarity(i, grains) for i in range(0, len(grains))]

# Probability to take the i-th grain:
def Pp(i, grains, ant):
    k1 = 0.5
    return (k1 / (k1 + similarity(i, grains, ant))) ** 2

# Probability to leave the i-th grain:
def Pd(i, grains, ant):
    k2 = 0.5
    print("similarity = " + str(similarity(i, grains, ant)))
    return (similarity(i, grains, ant) / (k2 + similarity(i, grains, ant))) ** 2

def actualizeAnts(ants, grains, cels):
    # print("Actualizing ants")
    for ant in ants:
        x = ant.rectangle.left//BLOCK_SIDE
        y = ant.rectangle.top//BLOCK_SIDE
        # Case 01 - If the is not carring any grain and there is a grain in it's actual cel:
        if (ant.carring_grain == None) and (cels[y][x].actual_grain != None):
            i = grains.index(cels[y][x].actual_grain)
            probability_to_take = Pp(i, grains, ant)
            print("calculated pp:")
            print(probability_to_take)
            result = random.random()
            if result <= probability_to_take:
                print("An ant took a grain!")
                print("y = " + str(y))
                print("x = " + str(x))
                ant.carring_grain = cels[y][x].actual_grain
                cels[y][x].actual_grain = None
        # Case 02 - If the ant is carring a grain and is at an empty cel:
        elif (ant.carring_grain != None) and (cels[y][x].actual_grain == None):
            i = grains.index(ant.carring_grain)
            probability_to_leave = Pd(i, grains, ant)
            print("calculated pd:")
            print(probability_to_leave)
            result = random.random()
            if result < probability_to_leave:
                cels[y][x].actual_grain = ant.carring_grain
                cels[y][x].actual_grain.rectangle.left = x * BLOCK_SIDE
                cels[y][x].actual_grain.rectangle.top = y * BLOCK_SIDE
                ant.carring_grain = None
        # Always move the ant:
        moveAnt(ant, cels)
    countGrainsAround(ants, cels, 2)

def moveAnt(ant, cels):
    x = ant.rectangle.left // BLOCK_SIDE 
    y = ant.rectangle.top // BLOCK_SIDE
    while True:
        dx = random.randint(-1, 1)
        dy = random.randint(-1, 1)
        new_x = max(0, min(WINDOW_WIDTH//BLOCK_SIDE - 1, x + dx))
        new_y = max(0, min(WINDOW_HEIGHT//BLOCK_SIDE - 1, y + dy))

        if cels[new_y][new_x].actual_ant == None:
            break
    
    # Clear the old cel:
    cels[y][x].actual_ant = None

    # Put the ant at the new cel:
    cels[new_y][new_x].actual_ant = ant
    # Change the ant location:
    ant.rectangle.left = new_x * BLOCK_SIDE
    ant.rectangle.top = new_y * BLOCK_SIDE
    
    # If the ant was carring a grain:
    if(ant.carring_grain != None):
        # Change the grain location to the same as the ant:
        ant.carring_grain.rectangle.left = new_x * BLOCK_SIDE
        ant.carring_grain.rectangle.top = new_y * BLOCK_SIDE
        # There is no need to put the grain on a new cel because while being carried by the ant...
        # ... the grain is not considered to be at any cel.

def countGrainsAround(ants, cels, max_vision):
    for ant in ants:
        ant.around_grains = []
    for vision in range(1, max_vision+1):
        countGrainsInVision(ants, cels, vision)
    
def countGrainsInVision(ants, cels, vision):
    possible_directions = [(-vision, 0), (0, -vision), (-vision, -vision), (vision, 0), (0, vision), (vision, vision), (vision, -vision), (-vision, vision)]
    for ant in ants:
        x = ant.rectangle.left // BLOCK_SIDE 
        y = ant.rectangle.top // BLOCK_SIDE
        for (dx, dy) in possible_directions:
            seen_x = x + dx
            seen_y = y + dy
            if (0 <= seen_x < WINDOW_WIDTH//BLOCK_SIDE) and (0 <= seen_y < WINDOW_HEIGHT//BLOCK_SIDE):
                if (cels[seen_y][seen_x].actual_grain != None):
                    ant.around_grains += 1
                    ant.around_grains.append(cels[seen_y][seen_x].actual_grain)

if __name__ == "__main__":
    pygame.init()

    
    cels = [[Cel() for x in range(0, WINDOW_WIDTH//BLOCK_SIDE)] for y in range(0, WINDOW_HEIGHT//BLOCK_SIDE)]
    grains = setGrains(cels, 5, 10)
    ants = setAnts(20, cels)

    ALPHA = averageDistance(grains)
    print("alpha = " + str(ALPHA))

    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
        random.seed(str(datetime.datetime.now()))
        # drawGrid()
        drawGridStuff(cels)
        actualizeAnts(ants, grains, cels)
        pygame.display.update()
