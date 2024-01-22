cmake_minimum_required(VERSION 3.21)
project(untitled2)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)
link_directories(lib)

add_executable(task_3 main.cpp glad.c camera.h shader.h mesh.h model.h)
target_link_libraries(task_3 glfw3.dll OPENGL32.LIB libassimp-5d.dll )
