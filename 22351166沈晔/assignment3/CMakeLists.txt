# CMakeList.txt: Vulkan-demo 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()



# glslc 编译 shader file
find_program(GLSLC_PROGRAM glslc REQUIRED)
execute_process(COMMAND ${GLSLC_PROGRAM} ${CMAKE_SOURCE_DIR}/shader/shader.vert -o ${CMAKE_SOURCE_DIR}/spv/vert.spv)
execute_process(COMMAND ${GLSLC_PROGRAM} ${CMAKE_SOURCE_DIR}/shader/shader.frag -o ${CMAKE_SOURCE_DIR}/spv/frag.spv)

# 项目和链接
project ("Vulkan-demo")

include_directories("C:/VulkanSDK/1.3.268.0/Include")
include_directories("C:/Users/Sy200/Libraries/glm-0.9.9.8/glm")
include_directories("C:/Users/Sy200/Libraries/glfw-3.3.8.bin.WIN64/glfw-3.3.8.bin.WIN64/include")

link_directories("C:/VulkanSDK/1.3.268.0/Lib")
link_directories("C:/Users/Sy200/Libraries/glfw-3.3.8.bin.WIN64/glfw-3.3.8.bin.WIN64/lib-vc2022")

# 将源代码添加到此项目的可执行文件。

aux_source_directory(external EXT)
aux_source_directory(header HEAD)
aux_source_directory(src SRC)

add_executable (Vulkan-demo ${HEAD} ${SRC} ${EXT} "main.cpp")

# 消除有中文的报错 && 默认库“MSVCRT”与其他库的使用冲突；请使用 /NODEFAULTLIB:library
if(MSVC)
  target_compile_options(Vulkan-demo PRIVATE /utf-8)
  set(CMAKE_EXE_LINKER_FLAGS /NODEFAULTLIB:"MSVCRT.lib")
endif()




target_link_libraries(Vulkan-demo PRIVATE vulkan-1.lib)
target_link_libraries(Vulkan-demo PRIVATE glfw3.lib)

# 设置可执行文件输出目录为当前源代码目录
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# 指定 C++ 版本
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Vulkan-demo PROPERTY CXX_STANDARD 20)
endif()
