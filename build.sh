#!/bin/bash

if [ ! -d build ]; then
  mkdir -p build;
fi

cmake -B build && cmake --build build
