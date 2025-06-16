alias build := build-test
build-test:
    cmake -B build -DCMAKE_BUILD_TYPE=Debug 
    cmake --build build

build-release:
    cmake -B build -DCMAKE_BUILD_TYPE=Release 
    cmake --build build

alias run := run-test
run-test:
    cmake -B build -DCMAKE_BUILD_TYPE=Debug
    cmake --build build
    #
    # =============================================================== 
    #
    ./build/flyingfish c testprogram.ff
    #
    # =============================================================== 
    #
    ./build/flyingfish r out.cff

run-release:
    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build
    #
    # =============================================================== 
    #
    ./build/flyingfish c testprogram.ff
    #
    # =============================================================== 
    #
    ./build/flyingfish r out.cff

debug-compile:
    gdb --args ./build/flyingfish c testprogram.ff

debug-run:
    gdb --args ./build/flyingfish r out.cff
