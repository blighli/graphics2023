## 环境搭建
> Ubuntu: 22.04, NVIDIA Driver: 535.86.05, OpenGL: 4.5

```shell
# ESSENTIAL
sudo apt-get install build-essential libgl1-mesa-dev

# GLFW
sudo apt-get install libglfw3-dev

# GLAD (gl_ver: 4.5 Profile: Compatibility as internal using)
# Order it in https://glad.dav1d.de/ 
# Replace ./src/glad.c ./include/glad/ ./include/KHR/

```

## Usage

- Render a triangle as default

```shell
# cd <project_dir>
mkdir build && cd build
cmake ..
make
# EXECUTE_NAME OPTION: window triangle rectangle
# cmake .. -D EXECUTE_NAME=window
# cmake .. -D EXECUTE_NAME=rectangle
```