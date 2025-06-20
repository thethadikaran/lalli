cmake_minimum_required(VERSION 3.20)

# set the project name and the language used
project(lalli C)

# specify the c standard
set(CMAKE_C_STANDARD 11)

# request cmake to search for library and if not found, terminate with error
find_package(SDL2 REQUIRED)

# specify the executable
add_executable(main src/main.c)


# linking the libraries needed for SDL2
target_link_libraries(main PRIVATE -lmingw32 -lSDL2main -lSDL2)