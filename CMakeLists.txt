cmake_minimum_required(VERSION 3.5)
project(not_yet_editor VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED true)

add_subdirectory(SDL)
add_subdirectory(SDL_ttf)

add_executable( ${PROJECT_NAME}
            header/Level_editor.h    source/level_editor.cpp
            header/SDL_prims.h       source/SDL_prims.cpp
)
target_include_directories( ${PROJECT_NAME} 
    PUBLIC header
    PUBLIC SDL/include
    PUBLIC SDL_ttf
)

target_link_libraries( ${PROJECT_NAME} PUBLIC SDL2::SDL2 SDL2_ttf::SDL2_ttf)



