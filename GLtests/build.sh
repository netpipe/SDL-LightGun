#!/bin/bash
g++ sdl2-main.cpp -lSDL2 -lGLU -lGL -o sdl2

g++ sdl-main.cpp -lSDL -lGLU -lGL -o sdl

g++ sdl2-main-ex2.cpp -lSDL2 -lGLU -lGL -o sdl2-2

g++ sdl2GL2.cpp -lSDL2 -lGLU -lGL -lSDL2_image -lSDL2_mixer
