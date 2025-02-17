if not exist "build" mkdir build

cmake -G "MinGW Makefiles" -B build
cmake --build build
