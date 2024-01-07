# 链接的外部库名称 参考相对应的 add_library(glm ...)
if(TARGET glm::glm)
    return()
endif()

include(FetchContent)
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG 0.9.9.8
)
FetchContent_MakeAvailable(glm)