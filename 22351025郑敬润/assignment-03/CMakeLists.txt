#REQUIRED 指定cmake最低版本 (FetchContent 需要 3.14+)
cmake_minimum_required(VERSION 3.16)

#REQUIRED 指定项目名及项目版本
project(LoadModel VERSION 1.2)

#OPTIONAL 指定C++标准
set(CMAKE_CXX_STANDARD 17) #需要string_view特性
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#REQUIRED 转换.h.in 为.h 头文件
configure_file(config.h.in config.h)

#REQUIRED 添加cmake目录到CMAKE_MODULE_PATH
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
#REQUIRED include方法：载入并运行来自于指定文件或模块CMAKE_MODULE_PATH下的glfw.cmake代码
include(glfw)
include(glm)
include(assimp)

#REQUIRED 添加可执行文件
add_executable(
        ${PROJECT_NAME}
        src/glad.c
        src/main.cpp
)

#REQUIRED 添加链接库
target_link_libraries(
        ${PROJECT_NAME} PUBLIC
        glfw
        glm::glm
        assimp::assimp
)

#REQUIRED 添加include路径
target_include_directories(
        ${PROJECT_NAME} PUBLIC
        include
        ${PROJECT_BINARY_DIR}
)