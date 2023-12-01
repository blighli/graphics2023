#REQUIRED 指定cmake最低版本 (FetchContent 需要 3.14+)
cmake_minimum_required(VERSION 3.16)

#UNIQUE 指定编译程序
    #默认编译triangle项目，Option：window, rectangle
if ("${EXECUTE_NAME}" STREQUAL "")
    set(EXECUTE_NAME triangle)
endif()


#REQUIRED 指定项目名及项目版本
project(${EXECUTE_NAME} VERSION 1.0)

#OPTION 指定C++标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)


#REQUIRED 添加可执行文件
    #文件名变量化声明，GLOB表示 SRC_FILES 为一个文件列表
file(GLOB SRC_FILES 
    src/glad.c 
    src/${EXECUTE_NAME}.cpp
    )
add_executable(${PROJECT_NAME} ${SRC_FILES})


#REQUIRED 添加include路径
include_directories(include)
#OPTION 添加额外include路径
set(SYS_INCLUDE_DIR /usr/local/include)
include_directories(${SYS_INCLUDE_DIR})

#OPTION 查找库
find_package(glfw3 REQUIRED)

#REQUIRED 添加链接库
target_link_libraries(${PROJECT_NAME}
    -lglfw
    )
