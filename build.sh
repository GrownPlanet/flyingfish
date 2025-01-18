#!/bin/bash

if [ ! -d build ]; then
  mkdir -p build;
fi

cd build\
    && cmake ..\
    && cmake --build .
