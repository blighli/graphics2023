cmake_minimum_required(VERSION 3.21)
project(untitled2)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)
link_directories(lib)

add_executable(work_2 main.cpp glad.c camera.h shader.h sphere.h)
target_link_libraries(work_2 glfw3.dll OPENGL32.LIB)
