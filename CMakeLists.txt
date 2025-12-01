cmake_minimum_required(VERSION 3.10)
project(l4s3r)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(libs/iorg)
add_subdirectory(libs/sorg)
add_subdirectory(libs/glfw-3.4)
add_subdirectory(libs/glad)

add_executable(l4s3r 
    main.cpp
    src/level.h
    src/level.cpp
    src/board.h
    src/board.cpp
    src/laser.h
    src/laser.cpp)

target_include_directories(l4s3r PUBLIC libs)
target_link_libraries(l4s3r iorg sorg glfw glad)
