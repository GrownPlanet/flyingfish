# Flying fish
A simple programming language written in c.

## Building and running
To build this program you will need a c compiler (gcc is used in the makefile but you can use 
another if you want to) and make, no external libraries are needed.\
To build: `make`\
To run: `{executable} {program}`, the executable can be found in `build/` and the test program is 
`makefile`

## File structure
- /src
    - /sourcefiles
        - (sourcefiles of the project)
    - /headerfiles
        - (headerfiles of the project)
    - /utils (these are utils that may be used throughout the program but don't belong to a specific 
part of the program)
        - /sourcefiles
        - /headerfiles
- makefile
- /build
    - (files from the build process)
