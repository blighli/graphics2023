# Compiler
CC = g++

EXE = a.out

SOURCES = main.cpp glad/glad.c stb_image.hpp

CXXFLAGS = -g -Wall -Wformat -std=c++11 -Wno-misleading-indentation -DSTB_IMAGE_IMPLEMENTATION

LIBS= -lglfw -lOpenGL -lassimp

all:
	$(CC) $(CXXFLAGS) $(SOURCES) -o $(EXE) $(LFLAGS) $(LIBS)

clean:
	rm $(EXE)
