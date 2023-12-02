# 编译器设置
CC = emcc

# 编译选项
CFLAGS = -std=c++11 -O3 -Wall

# SDL2和OpenGL编译选项
EFLAGS = -s USE_SDL=2 -s USE_WEBGL2=1 -s LEGACY_GL_EMULATION=1 -s ASYNCIFY

# 可执行文件名
OUTPUT = index.html

# 源文件
SRC = main.cpp

# 生成可执行文件的规则
$(OUTPUT): $(SRC)
	$(CC) $(SRC) -o $(OUTPUT) $(EFLAGS)

# 清理生成的文件
clean:
	rm -f $(OUTPUT)
