CC = g++
CFLAGS = -O3 -std=c++20 -Wall -Wextra -lglfw3 -lGL -lpthread

SRC += $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, src/%.o, ${SRC})

TARGET := build/window

.PHONY : test
test : ${OBJ}
${OBJ}:%.o:%.cpp
	$(CC) -c $< -o $@ $(CFLAGS)

.PHONY : build
build : 
	g++ -c src/glad.c -o src/glad.o $(CFLAGS) 
	$(CC) $(SRC) src/glad.o -o $(TARGET) $(CFLAGS) 

.PHONY : run
run : $(TARGET)
	./$(TARGET)

.PHONY br:
br : build run

.PHONY : fmt
fmt:
	clang-format -i $(shell find src -name '*.cpp')
	clang-format -i $(shell find src -name '*.hpp')

.PHONY : clean
clean:
	rm src/*.o

.PHONY : buildclean
buildclean:
	cd src/build && rm -rf *

.PHONY : convert
convert:
	convert  xxx.jpg  xxx.png

