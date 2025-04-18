#!/bin/bash

# A simple program to run the current program and test if it works
cmake -B build -DCMAKE_BUILD_TYPE=Debug\
    && cmake --build build\
    && echo -e "\n----------------------------------------------------------------\n"\
    && ./build/flyingfish c testprogram.ff\
    && echo -e "\n----------------------------------------------------------------\n"\
    && ./build/flyingfish r out.cff
