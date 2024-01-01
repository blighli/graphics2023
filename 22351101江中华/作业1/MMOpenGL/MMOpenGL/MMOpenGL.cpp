// MMOpenGL.cpp: 定义应用程序的入口点。
//

#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <MMGL/MMGL.hpp>

//using namespace std;

#define STRINGIZE(x)  #x
#define SHADER(shader) "" STRINGIZE(shader)

int main()
{
	printf("Hello MMOpenGL.\n");
	printf("江中华22351101.\n");
	printf("作业1.\n");

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int windowWidth = 1280;
	int windowHeight = 720;
	GLFWwindow * window = glfwCreateWindow(windowWidth, windowHeight, "jiang22351101", NULL, NULL);

	// OpenGL 运行模式单线程，与当前线程进行绑定
	// 如果要多个线程调用，线程调用前都要使用以下命令
	glfwMakeContextCurrent(window);

	// glad 初始化
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	//char* shaderStr = (char*)"int main(){}";
	char* vertexShader = SHADER(
		#version 330\n
		uniform float a;
		layout(location = 0) in vec3 pos; //传入的顶点
		layout(location = 1) in vec3 uvPos; // 纹理坐标
		out vec3 outPos; // 传出的顶点，n个
		out vec3 outUVPos; //传出的纹理坐标
		void main() {
			outPos = pos;
			outUVPos = uvPos;
			float _a = sin(a);
			//float _a = 1.0;
			gl_Position = vec4(pos.x*_a, pos.y*_a, pos.z*_a, 1.0);
		}
	);
	char* fragmentShader = SHADER(
		#version 330\n

		out vec4 rgbaColor;
		in vec3 outPos; // 接收光栅化后的顶点，不止n个
		in vec3 outUVPos; // 接收纹理坐标
		uniform sampler2D t; //一个Shader最多32个纹理单元
		void main() {
			// 设置纹理坐标
			vec2 uv = vec2(outUVPos.x, outUVPos.y);
			vec4 color = texture(t, uv);
			rgbaColor = vec4(outPos, 1.0); // 把位置信息当作颜色信息
			rgbaColor = color;
		}
	);

	float vertex[] = {	// 四个顶点
		-1.0, 1.0,  0.0, // A
		1.0,  1.0,  0.0, // B
		1.0,  -1.0, 0.0, // C
		-1.0, -1.0, 0.0  // D
	};

	float vertexUV[] = {	// 四个顶点
		0.0,  0.0,  0.0, // A
		1.0,  0.0,  0.0, // B
		1.0,  1.0,  0.0, // C
		0.0,  1.0,  0.0  // D
	};
	
	unsigned int index[] = {
		0, 3, 1, // ADB
		1, 3, 2  // BDC
	};

	// VAO
	MMGLVAO* vao = new MMGLVAO();
	vao->AddVertex3D(vertex, 4, 0);
	vao->AddVertex3D(vertexUV, 4, 1);
	vao->SetIndex(index, 6);
	MMGLProgram* program = new MMGLProgram(vertexShader, fragmentShader);

	int imgWidth = 2;
	int imgHeight = 2;
	unsigned char imgData[] = {
		255, 0, 0,   0, 255, 0,
		0, 0, 255,   127, 127, 127
	};

	GLuint textureId = 0;
	glGenTextures(1, &textureId); // 初始化 texture
	glBindTexture(GL_TEXTURE_2D, textureId); //绑定 texture

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //设置1字节对齐

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData);

	float aa = 0.0f;
	// 监听窗口是否关闭
	while (!glfwWindowShouldClose(window)) {
		// TODO 绘制操作
		glClear(GL_COLOR_BUFFER_BIT);	// 清空画布上的颜色buffer
		program->UseProgram();

		GLint loc = glGetUniformLocation(program->program, "a");
		glUniform1f(loc, aa);
		aa += 0.001;

		GLint textureLoc = glGetUniformLocation(program->program, "t");
		glActiveTexture(GL_TEXTURE0); // 激活0号纹理单元
		glBindTexture(GL_TEXTURE_2D, textureId); //绑定 texture 到 0号纹理单元
		glUniform1i(textureLoc, 0); // 传入0号纹理单元

		vao->Draw();
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	delete program;
	delete vao;
	// 释放 glfw 资源
	glfwTerminate();

	return 0;
}
