#!/usr/bin/env python3

#реализация линейного конгруэнтного метода
import random as rand

class LCG:
    def __init__(self, seed):
        self.seed = seed
        self.a = 1103515245
        self.c = 12345
        self.m = 2 ** 64

    def random(self):
        self.seed = (self.a * self.seed + self.c) % self.m
        return self.seed / self.m
    
random = LCG(rand.randint(123456789, 2**64))
print (random.random())