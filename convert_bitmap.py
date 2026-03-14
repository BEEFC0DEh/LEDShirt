#!/usr/bin/python
# -*- coding: utf-8 -*-
"""
This script converts a set of PNGs to a number of
FastLEG CRGB arrays in python format.
"""
from PIL import Image
import numpy as np


RED = (255, 0, 0)
YELLOW = (255, 255, 0)
GREEN = (0, 255, 0)
CYAN = (0, 255, 255)
BLUE = (0, 0, 255)
PURPLE = (180, 0, 255)
BROWN = (128, 64, 0)
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
GRAY = (192, 192, 192)
LIGHTGREEN = (128, 255, 128)

COLORS_MAP = {
    RED: "RED",
    YELLOW: "YELLOW",
    GREEN: "GREEN",
    BLUE: "BLUE",
    BROWN: "BROWN",
    BLACK: "BLACK",
    WHITE: "WHITE",
    GRAY: "GRAY",
    LIGHTGREEN: "LIGHTGREEN"
}

def load_frame(name, number):
    img = Image.open(f'./{name}{number}.png')
    pixels = np.array(img)
    pixels = list(map(tuple, pixels.reshape(-1, 4)))
    return pixels

def print_frame(pixels, number, zigzag = False):
    tuples_3d = [t[:3] for t in pixels]
    frame = f'FRAME{number} = [\n\t'
    row = []
    backwards = False
    for i in range(1, len(tuples_3d) + 1):
        color_string = COLORS_MAP.get(tuples_3d[i - 1], str(tuples_3d[i - 1]))
        if backwards:
            row += [', ', color_string]
        else:
            row += [color_string, ', ']
        if not (i % 8):
            if backwards:
                row = reversed(row)
            frame += ''.join(row)
            frame += '\n\t'
            row = []
            if zigzag:
                backwards = not backwards
    frame = frame[:-4] + "\n]"
    print(frame)

print("===Straight pixels order===")
for i in range(1, 3):
    result = load_frame('Tea', i)
    print_frame(result, i)

# print("===Zigzag pixels order===")
# for i in range(1, 3):
#     result = load_frame('Tea', i)
#     print_frame(result, i, True)