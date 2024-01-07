# 链接的外部库名称 参考相对应的 add_library(assimp::assimp ...)
if(TARGET assimp::assimp)
    return()
endif()

include(FetchContent)
FetchContent_Declare(
    assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v5.3.1
)
FetchContent_MakeAvailable(assimp)