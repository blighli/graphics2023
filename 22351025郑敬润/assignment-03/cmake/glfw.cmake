# 链接的外部库名称 参考相对应的 add_library(glfw ...)
if(TARGET glfw)
    return()
endif()

include(FetchContent)
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.9
)
FetchContent_MakeAvailable(glfw)