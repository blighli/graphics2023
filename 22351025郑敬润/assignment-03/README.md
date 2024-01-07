## 构建环境
> Tested on:
>
> - Ubuntu 22.04, g++ 11.4.0, NVIDIA Driver: 535.86.05, OpenGL: 4.5
> 
> - MacOS 13.6.1, Clang 15.0.0, M1 2020, OpenGL: 4.1
>
> - Windows10 22H2, MSVC2019 16.11.32, NVIDIA Driver: 536.23, OpenGL: 4.6
>

## Build

```shell
# Make sure CMake version 3.16+

# cd <project_dir>
cmake -S . -B build
cmake --build build
```

## Usage

- 支持加载并渲染 .obj 模型，并且具有多级光照和纹理贴图
- 通过 WASD 控制相机移动，鼠标移动控制视角
- 按下空格键切换手电筒效果开关
- 按下 F 键切换鼠标拖拽模式来观察模型，同时解除光标窗口锁定
- 按下 ESC 退出程序

```shell
# UNIX like:
./build/LoadModel
# Windows:
.\build\Debug\LoadModel.exe
```

## 3rd library

```shell
# ESSENTIAL
# Ubuntu:
sudo apt-get install build-essential libgl1-mesa-dev

# GLFW
# Obtained with CMake FetchContent; make sure github availability 

# GLM
# Obtained with CMake FetchContent; make sure github availability 

# assimp
# Obtained with CMake FetchContent; make sure github availability 

# GLAD
# (gl_ver: 4.6 Profile: Core) as internal using
# Order it in https://glad.dav1d.de/ 
# Replace ./src/glad.c ./include/glad/ ./include/KHR/

# stb_image.h
# located at include/stb_image.h as internal using
```


## Download model
> wget https://learnopengl-cn.github.io/data/nanosuit.rar
>  
> Unpackage to ./res/objects/nanosuit/