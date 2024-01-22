cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og")

project(assignment)

if(WIN32 AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fext-numeric-literals)
    if(NOT DEFINED WIN32_LEAN_AND_MEAN)
        add_definitions(-DWIN32_LEAN_AND_MEAN)
    endif()
    if(NOT DEFINED WINVER)
        add_definitions(-DWINVER=0x0A00)
    endif()
    if(NOT DEFINED _WIN32_WINNT)
        add_definitions(-D_WIN32_WINNT=0x0A00)
    endif()
    if(NOT DEFINED NTDDI_VERSION)
        add_definitions(-DNTDDI_VERSION=0x0A000007)
    endif()
endif()

add_executable(${PROJECT_NAME})
include(cmake/func.cmake)

if(NOT BASE_DIRECTORY)
    set(BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
endif()
if(EXISTS ${BASE_DIRECTORY}/nvpro_core/cmake/setup.cmake)
    include(3rdparty/nvpro_core/cmake/setup.cmake)
    include(3rdparty/nvpro_core/cmake/utilities.cmake)
endif()
set(OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin_${ARCH})

# library for graphics rendering
message(STATUS "-------------------------------")
_add_package_VulkanSDK()
_add_package_ImGUI()
_add_nvpro_core_lib()

# library for resource loading
message(STATUS "-------------------------------")
add_subdirectory(${BASE_DIRECTORY}/rapidobj)
message(STATUS "--> using package rapidobj")
add_subdirectory(${BASE_DIRECTORY}/alpaca)
message(STATUS "--> using package alpaca")

_add_project_definitions(${PROJECT_NAME})

# compile all GLSL shader to Spirv
message(STATUS "-------------------------------")
message(STATUS "compiling shaders to spirv...")
set(GLSL_COMPILE_FLAGS)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND GLSL_COMPILE_FLAGS -DDEBUG_MODE)
endif()
if(NOT EXISTS ${OUTPUT_PATH}/${CMAKE_BUILD_TYPE}/builtin_resources/shaders)
    file(MAKE_DIRECTORY ${OUTPUT_PATH}/${CMAKE_BUILD_TYPE}/builtin_resources/shaders)
endif()
compile_glsl_directory_by_shaderc(
    SRC "${CMAKE_CURRENT_SOURCE_DIR}/builtin_resources/shaders" 
    DST "${OUTPUT_PATH}/${CMAKE_BUILD_TYPE}/builtin_resources/shaders" 
    VULKAN_TARGET "vulkan1.3"
    DEPENDENCY ${VULKAN_BUILD_DEPENDENCIES}
    FLAGS ${GLSL_COMPILE_FLAGS}
    )

file(GLOB SOURCE_FILES src/*.cpp src/*.hpp)
list(REMOVE_ITEM SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/evalDefGenerator.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

source_group("shader files" FILES ${GLSL_SOURCES})
source_group("source files" FILES ${SOURCE_FILES})

target_sources(${PROJECT_NAME} PUBLIC ${SOURCE_FILES})
target_sources(${PROJECT_NAME} PUBLIC ${COMMON_SOURCE_FILES})
target_sources(${PROJECT_NAME} PUBLIC ${PACKAGE_SOURCE_FILES})
target_sources(${PROJECT_NAME} PUBLIC ${GLSL_SOURCES} ${GLSL_HEADERS})

target_link_libraries(${PROJECT_NAME} PUBLIC ${PLATFORM_LIBRARIES} nvpro_core) # link nvpro_core libraries
target_link_libraries(${PROJECT_NAME} PUBLIC ${PLATFORM_LIBRARIES} rapidobj::rapidobj) # link rapidobj libraries
target_link_libraries(${PROJECT_NAME} PUBLIC ${PLATFORM_LIBRARIES} alpaca) # link alpaca libraries

_finalize_target(${PROJECT_NAME})

add_custom_command(TARGET ${PROJECT_NAME}
                    POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_CURRENT_SOURCE_DIR}/builtin_resources/textures
                    ${OUTPUT_PATH}/${CMAKE_BUILD_TYPE}/builtin_resources/textures)
add_custom_command(TARGET ${PROJECT_NAME}
                    POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_CURRENT_SOURCE_DIR}/builtin_resources/models
                    ${OUTPUT_PATH}/${CMAKE_BUILD_TYPE}/builtin_resources/models)
add_custom_command(TARGET ${PROJECT_NAME}
                    POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory()
                    ${CMAKE_CURRENT_SOURCE_DIR}/cache)