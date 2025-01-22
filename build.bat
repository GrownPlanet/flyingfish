if not exist "build" mkdir build

REM I have not tested this yet
set CC=gcc

cd build
cmake -DCMAKE_C_COMPILER=gcc ..
cmake --build .
cd ..
