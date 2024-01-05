# CMakeList.txt: OpenglTest 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project("OpenglTest")
link_directories(${PROJECT_SOURCE_DIR}/lib)
message(STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17")
message(STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})
#option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
#option(ASSIMP_BUILD_ASSIMP_VIEW OFF)
#option(ASSIMP_BUILD_SAMPLES OFF)
#option(ASSIMP_BUILD_TESTS OFF)
#option(BUILD_SHARED_LIBS OFF)
#add_subdirectory(lib/assimp)
#set(SOURCE_FILES "Window.cpp" glad.c)
set(SOURCE_FILES "shader.h" "stb_image.h" "stb_image.cpp" glad.c "camera.h" "Mesh")
add_executable(main ${SOURCE_FILES} "Cubemaps.cpp")

include_directories(${PROJECT_SOURCE_DIR}/include)

target_link_libraries(main glfw3 assimp-vc142-mt)