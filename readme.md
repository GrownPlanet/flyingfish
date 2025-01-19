# Flying fish
A simple programming language written in c.

## Building and running
To build this program you will need a c compiler (gcc is used in the makefile but you can use 
another if you want to) and make, no external libraries are needed.\
To build: `./build.sh` on linux or `.\build.bat` on windows, you may need to change the compiler 
specefied in the bat file on windows\
To run the compiler:\
`{executable} [compile or c] INPUT`, the executable can be found in `build/` and the test program is 
`testprogram.ff`\
`{executable} [run or r] BYTECODE`, this will run the compiled bytecode

## File structure
- /src
    - /sourcefiles
        - (sourcefiles of the project)
    - /headerfiles
        - (headerfiles of the project)
- /build
    - (files from the build process)
- CmakeLists.txt
- testprogram.ff
