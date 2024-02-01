#!/bin/bash

# # -------- BUILD SETTINGS --------

C_FLAGS="-Wall -Wextra -pedantic -std=c++20 -masm=intel"
LINKER_FLAGS="-s"

Optimization_flags="-O3 -march=native -msse4.2"
# Optimization_flags="-ggdb -g3"


lib_inc_dir=Libs/
include_dir=Src/include
sdl_inc_dir=Libs/SDL2/include/SDL2/

# -------- BUILD SCRIPT --------
C_FLAGS="${C_FLAGS} ${Optimization_flags}"

set -x

rm Asm/*.s

g++ $C_FLAGS -I $include_dir -I $lib_inc_dir -I $sdl_inc_dir -S Src/main.cpp -o Asm/main.s
g++ $C_FLAGS -I $include_dir -I $lib_inc_dir -I $sdl_inc_dir -S Src/engine.cpp -o Asm/engine.s
g++ $C_FLAGS -I $include_dir -I $lib_inc_dir -I $sdl_inc_dir -S Src/utils.cpp -o Asm/utils.s
g++ $C_FLAGS -I $include_dir -I $lib_inc_dir -I $sdl_inc_dir -S Src/surface.cpp -o Asm/surface.s

g++ Asm/*.s -o LiRaster $LINKER_FLAGS -lSDL2
./LiRaster
