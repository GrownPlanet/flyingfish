# Flying fish
A simple, badly written and terribly optimized programming language written in c.

Don't try and use it because I broke something at some point and have no motivation left to 
fix it :(, but at least I learned something. Bye World!

## Building, running and testing
To build this program you will need a c compiler (gcc is specified in build.bat but you can use 
another compiler if you want to) and cmake, no external libraries are needed.\
To build:\
`cmake -B build -DCMAKE_BUILD_TYPE=Debug` and `cmake --build build` on linux, 
and windows (, and macos, but has not been verified yet), or use cmake in your preferred way\
To run the compiler:\
`{executable} [compile or c] [input file]`, the executable can be found in `build/` and the program
I use to test teh program is `testprogram.ff`\
`{executable} [run or r] [compiled bytecode]`, this will run the compiled bytecode.\
To test:\
You can use `./run.sh` to compile and run the program on linux (and macos?) and 
`pwsh -File ./run.ps1` to run it on windows.

## File structure
- /src
    - /sourcefiles
        - (sourcefiles of the project)
    - /headerfiles
        - (headerfiles of the project)
- /build
    - (files from the build process)
- CmakeLists.txt
