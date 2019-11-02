#!/bin/bash

# change to the build directory
mkdir -p ./build
cd ./build

# generate the build files
# cmake .. -DCMAKE_TOOLCHAIN_FILE=/home/abhishek/code/vcpkg/scripts/buildsystems/vcpkg.cmake -DENABLE_TESTS=On
cmake .. -DENABLE_TESTS=On

# go back to parent directory
cd ..
