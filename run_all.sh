#!/usr/bin/env bash

cmake -B build
cmake --build build

# Run all testcases. 
# You can comment some lines to disable the run of specific examples.
mkdir -p output
build/RayTracing output/scene00.bmp 8
# build/RayTracing testcases/scene01_basic.txt output/scene01.bmp
# build/RayTracing testcases/scene02_cube.txt output/scene02.bmp
# build/RayTracing testcases/scene03_sphere.txt output/scene03.bmp
# build/RayTracing testcases/scene04_axes.txt output/scene04.bmp
# build/RayTracing testcases/scene05_bunny_200.txt output/scene05.bmp
# build/RayTracing testcases/scene06_bunny_1k.txt output/scene06.bmp
# build/RayTracing testcases/scene07_shine.txt output/scene07.bmp
