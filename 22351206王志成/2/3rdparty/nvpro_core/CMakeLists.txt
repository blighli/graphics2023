cmake_minimum_required(VERSION 3.5)
#####################################################################################
# offer the choice of having nvpro_core as a sub-folder... good for packaging a sample
#
# if BASE_DIRECTORY not defined, it means this cmake file was called as the first entry point and not included
if(NOT BASE_DIRECTORY) # if not defined, it means this cmake file was called as the first entry point and not included
  message(FATAL_ERROR "cannot be directly invoked")
else()
  Message(STATUS "-------------------------------")
  Message(STATUS "Processing nvpro_core files")
endif()


set(NVPWINDOW_SOURCE
  ${BASE_DIRECTORY}/nvpro_core/nvp/nvpwindow.cpp
  ${BASE_DIRECTORY}/nvpro_core/nvp/nvpwindow.hpp
  ${BASE_DIRECTORY}/nvpro_core/nvp/nvpsystem.cpp
  ${BASE_DIRECTORY}/nvpro_core/nvp/nvpsystem.hpp
  ${BASE_DIRECTORY}/nvpro_core/nvp/nvpfilesystem.cpp
  ${BASE_DIRECTORY}/nvpro_core/nvp/nvpfilesystem.hpp
  # Do not add project_name.cpp here as it contains per-project dependent data.
  # perproject_globals.cpp will be automatically added to the per-project builds via
  # COMMON_SOURCE_FILES
  ${BASE_DIRECTORY}/nvpro_core/nvp/perproject_globals.hpp
)

if(UNIX) 
  LIST(APPEND NVPWINDOW_SOURCE ${BASE_DIRECTORY}/nvpro_core/nvp/nvpsystem_linux.cpp )
else()
  LIST(APPEND NVPWINDOW_SOURCE ${BASE_DIRECTORY}/nvpro_core/nvp/nvpsystem_win32.cpp )
endif()

if(NOT (CMAKE_SIZEOF_VOID_P EQUAL 8))
  message( FATAL_ERROR "64-bit builds are mandatory for this framework" )
endif()


# Add third party libraries. This sets THIRDPARTY_LIBRARIES to list of added libraries
add_subdirectory(third_party)


# add packages that may be needed by some of the samples
_optional_package_VulkanSDK()
_optional_package_ShaderC()
_optional_package_Freeimage()
_optional_package_Cuda()
_optional_package_NVToolsExt()

# process the rest of some cmake code that needs to be done after the packages
_process_shared_cmake_code()

file(GLOB FILEFORMATS_SOURCE fileformats/*.cpp fileformats/*.hpp fileformats/*.inl fileformats/*.h)
file(GLOB NVMATH_SOURCE nvmath/*.cpp nvmath/*.hpp nvmath/*.inl nvmath/*.h)
file(GLOB NVHELPERS_SOURCE nvh/*.cpp nvh/*.hpp nvh/*.inl nvh/*.h)

if (NOT TARGET tinygltf)
  list(REMOVE_ITEM NVHELPERS_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/nvh/gltfscene.cpp)
  list(REMOVE_ITEM NVHELPERS_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/nvh/gltfscene.hpp)
  list(REMOVE_ITEM FILEFORMATS_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/fileformats/tiny_gltf_freeimage.h)
endif()

set(library_name "nvpro_core")

if(USING_VULKANSDK)
  file(GLOB NVHELPERSVK_SOURCE nvvk/*.cpp nvvk/*.hpp nvvk/*.inl nvvk/*.h)

  # the implementation depends on SUPPORTS_AFTERMATH and thus can only be provided
  # as part of the individual sample projects. Check setup.cmake where we insert
  # nsight_aftermath_vk.cpp into the projects automatically
  list(REMOVE_ITEM NVHELPERSVK_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/nvvk/nsight_aftermath_vk.cpp)

  # Adding compile_glsl 
  if(EXISTS ${BASE_DIRECTORY}/nvpro_core/cmake/utilities.cmake)
    include(${BASE_DIRECTORY}/nvpro_core/cmake/utilities.cmake)
  endif()

  # Vulkan-high-level helpers
  file(GLOB NVVKHL_SOURCE nvvkhl/*.cpp nvvkhl/*.hpp nvvkl/*.inl nvvkh;/*.h)
  set(SHD_DIR nvvkhl/shaders)
  file(GLOB SHD_HDR  ${SHD_DIR}/*.glsl ${SHD_DIR}/*.h)
  file(GLOB SHD_SRC ${SHD_DIR}/*.vert ${SHD_DIR}/*.frag ${SHD_DIR}/*.comp ${SHD_DIR}/*.rgen ${SHD_DIR}/*.rchit ${SHD_DIR}/*.rmiss)
  # Compiling shaders to SPIR-V header
  compile_glsl(
      SOURCE_FILES ${SHD_SRC}
      HEADER_FILES ${SHD_HDR}
      DST "${CMAKE_CURRENT_SOURCE_DIR}/_autogen" 
      VULKAN_TARGET "vulkan1.2"
      HEADER ON
      DEPENDENCY ${VULKAN_BUILD_DEPENDENCIES}
      FLAGS "-I${SHD_DIR}" "-I${CMAKE_CURRENT_SOURCE_DIR}" -g
      )

  if (NOT TARGET vma)
    list(REMOVE_ITEM NVHELPERSVK_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/nvvk/memallocator_vma_vk.hpp)
    list(REMOVE_ITEM NVHELPERSVK_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/nvvk/memallocator_vma_vk.inl)
    list(REMOVE_ITEM NVVKHL_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/nvvkhl/alloc_vma.hpp)
  endif()
  
  source_group("nvvk" FILES ${NVHELPERSVK_SOURCE})
  source_group("nvvkhl" FILES ${NVVKHL_SOURCE})
  source_group("nvvkhl/shaders" FILES ${GLSL_SOURCES} ${GLSL_HEADERS})
  set(NVVKHL_SHADERS ${GLSL_SOURCES} ${GLSL_HEADERS})

  string(CONCAT library_name ${library_name} "_vk")
  
  if(WIN32)
    add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)
  endif(WIN32)
  if(UNIX)
    add_definitions(-DVK_USE_PLATFORM_XCB_KHR)
    add_definitions(-DVK_USE_PLATFORM_XLIB_KHR)
  endif(UNIX)
endif()

if(USING_IMGUI)
  set(IMGUI_SOURCE 
      imgui/imgui_axis.cpp  
      imgui/imgui_axis.hpp
      imgui/imgui_camera_widget.cpp 
      imgui/imgui_camera_widget.h 
      imgui/imgui_helper.cpp
      imgui/imgui_helper.h
      imgui/imgui_orient.cpp  
      imgui/imgui_orient.h
      ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
      ${IMGUI_DIR}/backends/imgui_impl_glfw.h
      )
  if (NOT TARGET tinygltf)
    # depends on json.hpp from tinygltf
    list(REMOVE_ITEM IMGUI_SOURCE imgui/imgui_camera_widget.h)
    list(REMOVE_ITEM IMGUI_SOURCE imgui/imgui_camera_widget.cpp)
  endif()

  # pickup our own backend directory
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/imgui ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends)
 
  if(USING_VULKANSDK)
    # replaced with our own copy
    list(REMOVE_ITEM IMGUI_SOURCE ${IMGUI_DIR}/imgui/backends/imgui_impl_vulkan.cpp)
    list(REMOVE_ITEM IMGUI_SOURCE ${IMGUI_DIR}/imgui/backends/imgui_impl_vulkan.h)

    list(APPEND IMGUI_SOURCE 
         ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_vk_extra.cpp 
         ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_vk_extra.h
         ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_vulkan.cpp 
         ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_vulkan.h)
  endif()
endif()

if (USING_SHADERC)
  string(CONCAT library_name ${library_name} "_shaderc")
endif()

source_group("fileformats" FILES ${FILEFORMATS_SOURCE})
source_group("noise" FILES ${NOISE_SOURCE})
source_group("nvmath" FILES ${NVMATH_SOURCE})
source_group("nvh" FILES ${NVHELPERS_SOURCE})
source_group("nvp" FILES ${NVPWINDOW_SOURCE})
source_group("imgui" FILES ${IMGUI_SOURCE})

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

add_library(nvpro_core STATIC 
  ${NVHELPERS_SOURCE}
  ${NVMATH_SOURCE}
  ${IMGUI_SOURCE}
  ${FILEFORMATS_SOURCE}
  ${NVPWINDOW_SOURCE}
  ${NVHELPERSVK_SOURCE}
  ${NVVKHL_SOURCE}
  ${NVVKHL_SHADERS}
  ${PACKAGE_SOURCE_FILES}
)

# Create precompiled header (added in CMake 3.16)
if((${CMAKE_VERSION} VERSION_GREATER "3.16.0") OR (${CMAKE_VERSION} VERSION_EQUAL "3.16.0"))
  target_precompile_headers(nvpro_core PRIVATE
    "<algorithm>"
    ${VULKAN_HEADERS}
  )
endif()

# Add X11 for screenshot support on linux
if(UNIX)
  find_package(X11 REQUIRED)
  LIST(APPEND PLATFORM_LIBRARIES ${X11_LIBRARIES})
endif()

if(USING_IMGUI)
  # Enable ImVec* operators for all nvpro_core. See imgui.h.
  target_compile_definitions(nvpro_core PRIVATE IMGUI_DEFINE_MATH_OPERATORS)
endif()

target_link_libraries(nvpro_core
  ${PLATFORM_LIBRARIES} 
  glfw 
  imgui
  ${THIRDPARTY_LIBRARIES}
 )

set_target_properties(nvpro_core PROPERTIES OUTPUT_NAME ${library_name})
_set_target_output(nvpro_core)

message(STATUS "nvpro_core library name: ${library_name}")

