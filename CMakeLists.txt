cmake_minimum_required(VERSION 3.24)
project(Chip8)

set(CMAKE_CXX_STANDARD 17)

include_directories(SDL/include)
link_directories(SDL/lib)

find_library(SDL2_LIB SDL2 SDL/lib)
find_library(SDL2_MAIN_LIB SDL2main SDL/lib)
find_library(MINGW_LIB mingw32 ${PROJECT_SOURCE_DIR})

add_executable(Chip8 main.cpp chip8.cpp console_interface.cpp)

target_link_libraries(Chip8 ${mingw32} ${MINGW_LIB} ${SDL2_MAIN_LIB}  ${SDL2_LIB} )

