CMAKE_MINIMUM_REQUIRED(VERSION 3.8)

PROJECT ("ZjuAnimation_Project3")

SET(CMAKE_CXX_STANDARD 17)

SET(3rd_PARTY_DIR "E:/Work/vs2019Work/Third_Party")
SET(OpenCV_DIR "${3rd_PARTY_DIR}/opencv/install")
SET(ASSIMP_DIR "${3rd_PARTY_DIR}/assimp/build/Release/install/lib/cmake/assimp-5.2")

FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(ASSIMP REQUIRED)

# root src directory
SET(root_src_dir "${PROJECT_SOURCE_DIR}/src")

# include directory
SET(includes_dir "${PROJECT_SOURCE_DIR}/includes")

# library directory
SET(lib_dir "${PROJECT_SOURCE_DIR}/lib")

# add include directory path
INCLUDE_DIRECTORIES(${includes_dir})

# add link library directory
LINK_DIRECTORIES(${lib_dir})

# add root src subdirectory
ADD_SUBDIRECTORY(${root_src_dir})

