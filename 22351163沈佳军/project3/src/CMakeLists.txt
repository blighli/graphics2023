# opengl link libraries
SET(opengl_libs glfw3.lib opengl32.lib)

# set executable output path to bin/
SET(EXECUTABLE_OUTPUT_PATH "bin/")

FILE(GLOB_RECURSE source_files "*.c" "*.cpp")

FILE(GLOB_RECURSE header_files "*.h" "*.hpp")

# include all the *.h file in current directory
INCLUDE_DIRECTORIES(${header_files})

# get the parent directory name
GET_FILENAME_COMPONENT(parent_dir_name project3 NAME)

# set the name of executable program as parent dir name and execute all the source file
ADD_EXECUTABLE(${parent_dir_name} ${source_files})

# link libraries when running
TARGET_LINK_LIBRARIES(
        ${parent_dir_name}
        ${opengl_libs}
        ${OpenCV_LIBS}
        ${ASSIMP_LIBRARIES}
)