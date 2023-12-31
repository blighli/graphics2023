# Assignment 1

## 编译运行

由于 WSL Arch 的 OpenGL 出现问题，只能迁移到 Windows了。

需要先更改 cmake 中的几个依赖项（`glad` `glfw` `glm`）的路径，然后再编译运行
```shell
$ mkdir build
$ cmake -B ./build .
$ cmake --build ./build --config=Debug
```

```shell
$ build\Debug\opengl-demo.exe
```