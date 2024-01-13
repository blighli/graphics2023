#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL2\soil2.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stack>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp> // glm::value_ptr
#include <glm\gtc\matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "Utils.h"
#include "Sphere.h"
using namespace std;

#define numVAOs 1
#define numVBOs 3

float cameraX, cameraY, cameraZ;
float sphLocX, sphLocY, sphLocZ;
GLuint renderingProgram;
GLuint renderingFont;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint earthTexture;
GLuint moonTexture;
GLuint sunTexture;
GLuint earthFontTexture;
GLuint sunFontTexture;
float rotAmt = 0.0f;

// variable allocation for display
GLuint mvLoc, projLoc,modelLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;
GLuint VBOf, EBOf, VAOf;
//stack<glm::mat4> mvStack;
stack<glm::mat4> mStack;
Sphere mySphere = Sphere(48);

void setupVertices(void) {
	std::vector<int> ind = mySphere.getIndices();
	std::vector<glm::vec3> vert = mySphere.getVertices();
	std::vector<glm::vec2> tex = mySphere.getTexCoords();
	std::vector<glm::vec3> norm = mySphere.getNormals();

	std::vector<float> pvalues;	//顶点位置
	std::vector<float> tvalues;	//纹理坐标
	std::vector<float> nvalues;	//法向量

	int numIndices = mySphere.getNumIndices();
	for (int i = 0; i < numIndices; i++) {
		pvalues.push_back((vert[ind[i]]).x);
		pvalues.push_back((vert[ind[i]]).y);
		pvalues.push_back((vert[ind[i]]).z);
		tvalues.push_back((tex[ind[i]]).s);
		tvalues.push_back((tex[ind[i]]).t);
		nvalues.push_back((norm[ind[i]]).x);
		nvalues.push_back((norm[ind[i]]).y);
		nvalues.push_back((norm[ind[i]]).z);
	}

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);


	GLfloat vertices[] =
	{
		-0.35-0.1f, 0.2-0.05f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.35+0.1f, 0.2-0.05f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-0.35-0.1f, 0.2+0.05f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.35+0.1f, 0.2+0.05f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f
	};
	GLuint indices[] =
	{
		0, 1, 2,        //用前3个顶点绘制第一个三角形
		1, 3, 2         //用后3个顶点绘制第二个三角形
	};
	
	glGenVertexArrays(1, &VAOf);
	glBindVertexArray(VAOf);
	glGenBuffers(1, &VBOf);
	glGenBuffers(1, &EBOf);

	glBindBuffer(GL_ARRAY_BUFFER, VBOf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOf);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	
	
	
}

void init(GLFWwindow* window) {
	renderingProgram = Utils::createShaderProgram("vertShader.vert", "fragShader.frag");
	renderingFont= Utils::createShaderProgram("fontShader.vert", "FontShader.frag");
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 2.0f;
	sphLocX = 0.0f; sphLocY = 0.0f; sphLocZ = -1.0f;

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	setupVertices();
	sunTexture = Utils::loadTexture("sun.jpg");
	earthTexture = Utils::loadTexture("earth.jpg");
	moonTexture = Utils::loadTexture("moon.png");
	sunFontTexture = Utils::loadTexture("font1.bmp");
	earthFontTexture= Utils::loadTexture("font2.bmp");
}

void display(GLFWwindow* window, double currentTime) {
	
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	//---------------font------------
	glUseProgram(renderingFont);
	if (_obj == ClickedObject::Sun)
	{
		mMat = glm::mat4(1.0f);
		vMat = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.5f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 mvMat1 = vMat * mMat;
		mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");//返回查询的统一变量的位置
		projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
		glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat1));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

		glBindBuffer(GL_ARRAY_BUFFER, VBOf);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOf);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sunFontTexture);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);


		glBindVertexArray(VAOf);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//cout << showTimes<<endl;
		cout << "Sun" << endl;
		showTimes--;
	}
	else if (_obj == ClickedObject::Earth) {
		mMat = glm::mat4(1.0f);
		vMat = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.5f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 mvMat1 = vMat * mMat;
		mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");//返回查询的统一变量的位置
		projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
		glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat1));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

		glBindBuffer(GL_ARRAY_BUFFER, VBOf);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOf);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, earthFontTexture);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);


		glBindVertexArray(VAOf);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		cout << "Earth" << endl;
		showTimes--;
	}
	if (showTimes == 0)
	{
		_obj = ClickedObject::None;
	}
	

	//------------------------------
	glUseProgram(renderingProgram);
	GLuint lightLoc = glGetUniformLocation(renderingProgram, "lightPos");
	glUniform3f(lightLoc, 0.0f, 0.0f, 0.0f);
	GLuint isSun= glGetUniformLocation(renderingProgram, "isSun");
	

	modelLoc= glGetUniformLocation(renderingProgram, "model");//返回查询的统一变量的位置
	
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");//返回查询的统一变量的位置
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	//vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	vMat = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.5f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	//mvStack.push(vMat);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	// ----------------------sun
	mMat = glm::mat4(1.0f);
	//第121行代码修改如下
	//
	//mMat = glm::rotate(mMat,0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
	//自转
	mMat *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0));
	//缩小
	mMat *= scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));
	//视角不变
	//vMat=glm::translate(vMat, glm::vec3(0.0f, 0.0f, -3.0f));
	glm::mat4 mvMat =  vMat*mMat;
	glUniform1f(isSun, 1.0f);
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sunTexture);

	glEnable(GL_CULL_FACE);		//调整深度
	glFrontFace(GL_CCW);

	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	//mvStack.pop();

	//-----------------------planet  
	mMat = glm::mat4(1.0f);
	glUniform1f(isSun, -1.0f);
	//公转1
	earthpos = glm::vec3(sin((float)currentTime) * 1.0, 0.0f, cos((float)currentTime) * 1.0);
	mMat*= glm::translate(glm::mat4(1.0f), earthpos);

	//自转
	mMat *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0));
	//缩小
	mMat *= scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
	//平移到边边去
	//mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0f, -5.0));
	//视角不变
	mvMat = vMat * mMat;

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTexture);

	glEnable(GL_CULL_FACE);		//调整深度
	glFrontFace(GL_CCW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	//mvStack.pop();

	//-----------------------moon
	mMat = glm::mat4(1.0f);
	
	
	//公转2
	mMat*= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime * 7) * 0.15, 0.0f, cos((float)currentTime * 7) * 0.15));
	//公转1
	mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime) * 1.0, 0.0f, cos((float)currentTime) * 1.0));
	//自转
	mMat *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0));
	//平移到边边去
	mMat *= scale(glm::mat4(1.0f), glm::vec3(0.02f, 0.02f, 0.02f));
	//视角不变
	mvMat =  vMat* mMat;
	
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, moonTexture);

	glEnable(GL_CULL_FACE);		//调整深度
	glFrontFace(GL_CCW);

	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	//mvStack.pop();
}

void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

void mouse_callback(GLFWwindow* window, int button, int action, int mods) {
	//
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	
	if (action == GLFW_PRESS) switch(button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		Utils::CastRay(xpos, ypos);
		std::cout << "xpos:" << xpos << "   ypos:" << ypos << std::endl;
		break;
	}
	
}
int main(void) {
	glewExperimental = GL_TRUE;
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(1000, 600, "test", NULL, NULL);

	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);
	
	glfwSetWindowSizeCallback(window, window_size_callback);
	//使用回调
	glfwSetMouseButtonCallback(window, mouse_callback);
	init(window);

	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
