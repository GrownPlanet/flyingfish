cmake_minimum_required(VERSION 3.10)

project(flyingfish C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
else()
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -Wall -Wextra -g -pedantic")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -Wall -Wextra -O3 -DNDEBUG -pedantic")
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/src/headerfiles
)

file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/src/sourcefiles/*.c
)

add_executable(flyingfish ${SOURCES})
