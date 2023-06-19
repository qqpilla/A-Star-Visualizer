cmake_minimum_required(VERSION 3.14)
project(A_STAR_VISUALIZER LANGUAGES C CXX)

set(sources 
    ./src/cost_queue.cpp
    ./src/grid.cpp
    ./src/main.cpp
    ./src/searcher.cpp
    ./src/shader_program.cpp
)

add_subdirectory(./external/glfw)
add_subdirectory(./external/glad)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders_dir.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/include/shaders_dir.h
)

add_executable(program ${sources})
target_include_directories(program 
PRIVATE
    ./include
)
target_link_libraries(program
PRIVATE
    glfw
    glad
)