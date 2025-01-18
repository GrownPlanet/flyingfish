if not exist "build" mkdir build

cd build
cmake -DCMAKE_C_COMPILER=gcc ..
cmake --build .
cd ..
