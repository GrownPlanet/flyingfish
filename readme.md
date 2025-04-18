# Flying fish
A simple, badly written and terribly optimized programming language written in c.

## Building and running
To build this program you will need a c compiler (gcc is specefied in build.bat but you can use 
another compiler if you want to) and cmake, no external libraries are needed.\
To build: `cmake -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build` on linux (and macos, has
not been verified yet) and `.\build.bat` on windows or use cmake in your prefered way\
To run the compiler:\
`{executable} [compile or c] [input file]`, the executable can be found in `build/` and the test program is
`testprogram.ff`\
`{executable} [run or r] [compiled bytecode]`, this will run the compiled bytecode.\
Alternatively, you can use `./run.sh` to compile and run the program on linux (and macos?);

## File structure
- /src
    - /sourcefiles
        - (sourcefiles of the project)
    - /headerfiles
        - (headerfiles of the project)
- /build
    - (files from the build process)
- CmakeLists.txt
