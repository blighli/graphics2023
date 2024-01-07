cmake_minimum_required(VERSION 3.17)
project(Curriculumdesign1)

find_package(OpenGL REQUIRED)

#macro(ADD_OSX_FRAMEWORK fwname target)
#    find_library(FRAMEWORK_${fwname}
#            NAMES ${fwname}
#            PATHS ${CMAKE_OSX_SYSROOT}/System/Library
#            PATH_SUFFIXES Frameworks
#            NO_DEFAULT_PATH)
#    if( ${FRAMEWORK_${fwname}} STREQUAL FRAMEWORK_${fwname}-NOTFOUND)
#        MESSAGE(ERROR ": Framework ${fwname} not found")
#    else()
#        target_link_libraries(${target}  "${FRAMEWORK_${fwname}}/${fwname}")
#        MESSAGE(STATUS "Framework ${fwname} found at ${FRAMEWORK_${fwname}}")
#    endif()
#endmacro(ADD_OSX_FRAMEWORK)

set(CMAKE_CXX_STANDARD 11)

if (APPLE)
    add_definitions(-Wno-deprecated-declarations)
endif()

# 执行编译命令
add_executable(Curriculumdesign1 Curriculumdesign.cpp Model.cpp glad.c stb_image.cpp stb_image.h)
# 添加头文件
set(OPT_H /opt/Homebrew/include)
target_include_directories(Curriculumdesign1 PUBLIC ${OPT_H})
target_include_directories(Curriculumdesign1 PUBLIC ${OPENGL_INCLUDE_DIR})
# 添加目标链接
set(GLEW_LINK /opt/Homebrew/lib/libGLEW.dylib)
set(GLFW_LINK /opt/Homebrew/lib/libglfw.dylib)
target_link_libraries(Curriculumdesign1 ${GLEW_LINK} ${GLFW_LINK})
target_link_libraries(Curriculumdesign1 ${OPENGL_LIBRARIES})
