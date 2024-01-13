#include "glew/glew.h"
#include "glfw/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Utils.h"
#include "ImportedModel.h"
#include "camera.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

static const int screen_width = 1920;
static const int screen_height = 1080;

static const int numVAOs = 1;
static const int numVBOs = 3;

static const float pai = 3.1415926f;

GLuint renderingProgram = 0;
GLuint vao[numVAOs] = { 0 };
GLuint vbo[numVBOs] = { 0 };

int width = 0;
int height = 0;
float aspect = 0.f;
float objLocX = 0.f, objLocY = 0.f, objLocZ = 0.f;
GLuint mvLoc = 0;
GLuint projLoc = 0;

GLuint shuttleTextureId = 0;

glm::mat4 mMat(1.f), vMat(1.f), pMat(1.f), mvMat(1.f);

Camera camera(glm::vec3(0.f, 0.f, 2.f));
float cameraX, cameraY, cameraZ;
ImportedModel myModel("resources/shuttle.obj");

GLboolean keys[1024] = { GL_FALSE };
GLboolean firstMouse = GL_TRUE;
float deltaTime = 0.f;
float lastFrame = 0.f;
float lastLocX = 0.f;
float lastLocY = 0.f;

float toRadians(float degrees)
{
	return (degrees * 2.f * pai) / 360.f;
}

void setupVertices(void)
{
	vector<glm::vec3> vert = myModel.getVertices();
	vector<glm::vec2> text = myModel.getTextureCoords();
	vector<glm::vec3> norm = myModel.getNormals();

	vector<float> pValues;
	vector<float> tValues;
	vector<float> nValues;

	for (int i=0; i< myModel.getNumVertices(); i++)
	{
		/*pValues.push_back(vert[i * 3 + 0].x);
		pValues.push_back(vert[i * 3 + 1].y);
		pValues.push_back(vert[i * 3 + 2].z);

		tValues.push_back(text[i * 2 + 0].s);
		tValues.push_back(text[i * 2 + 1].t);

		nValues.push_back(norm[i * 3 + 0].x);
		nValues.push_back(norm[i * 3 + 1].y);
		nValues.push_back(norm[i * 3 + 2].z);*/

		pValues.push_back(vert[i].x);
		pValues.push_back(vert[i].y);
		pValues.push_back(vert[i].z);

		tValues.push_back(text[i].s);
		tValues.push_back(text[i].t);

		nValues.push_back(norm[i].x);
		nValues.push_back(norm[i].y);
		nValues.push_back(norm[i].z);
	}

	glGenVertexArrays(numVAOs, vao);
	glBindVertexArray(vao[0]);

	glGenBuffers(numVBOs, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pValues.size() * sizeof(float), &(pValues[0]), GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, myModel.getVertices().size() * sizeof(float), &(pVlaues[0]), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, tValues.size() * sizeof(float), &(tValues[0]), GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, myModel.getTextureCoords().size() * sizeof(float), &(tValues[0]), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, nValues.size() * sizeof(float), &(nValues[0]), GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, myModel.getNormals().size() * sizeof(float), &(nValues[0]), GL_STATIC_DRAW);
}

void init(GLFWwindow* window)
{
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");
	//glfwGetWindowSize(window, &width, &height);
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 2.f;
	objLocX = 0.0f; objLocY = 0.0f; objLocZ = 0.0f;

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(toRadians(45.f), aspect, 0.01f, 1000.f);
	
	setupVertices();
	shuttleTextureId = Utils::loadTexture("resources/spstob_1.jpg");

	/*lastLocX = (float)screen_width / 2.f;
	lastLocY = (float)screen_height / 2.f;*/
}


void window_size_callback(GLFWwindow* window, int newWidth, int newHeight)
{
	//屏幕坐标和窗口的帧缓冲
	/*GLFW在这里和这里解释文档中的两个坐标系。
		简而言之，窗口坐标是相对于监视器和 / 或窗口的，并且以不一定对应于真实屏幕像素的人造单元给出。 当DPI缩放被激活时（例如，在带有视网膜显示器的Mac上），情况尤其如此。
		与窗口坐标相比，帧缓冲区的大小与像素相关，以便与glViewport OpenGLs要求相匹配。
		请注意，在某些系统上，窗口坐标和像素坐标可以相同，但这不一定是正确的。*/
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);

	cameraX = 0.0f; cameraY = 0.0f; cameraZ =4.f;
	objLocX = 0.0f; objLocY = 0.0f; objLocZ = 0.0f;
	mMat = glm::translate(glm::mat4(1.f), glm::vec3(objLocX, objLocY, objLocZ));
	//pMat = glm::perspective(glm::radians(45.f), aspect, 0.001f, 1000.f);
	pMat = glm::perspective(camera.Zoom, aspect, 0.001f, 1000.f);
}

void do_movement(void)
{
	if (keys[GLFW_KEY_W])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (keys[GLFW_KEY_S])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (keys[GLFW_KEY_A])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (keys[GLFW_KEY_D])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

void display(GLFWwindow* window, double currentTime)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.5f, 0.5f, 1.f);

	//启动着色器程序,在GPU上安装GLSL代码,这不会运行着色器程序，
	glUseProgram(renderingProgram);

	deltaTime = currentTime - lastFrame;
	lastFrame = currentTime;

	do_movement();

	//获取uniform变量在着色器程序中的位置序号，通过该序号可以设置一致变量的值，如果没有该变量则返回-1
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

	mMat = glm::translate(glm::mat4(1.f), glm::vec3(objLocX, objLocY, objLocZ));
	//vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	vMat = camera.GetViewMatrix();


	mMat = glm::rotate(mMat, toRadians(0.f), glm::vec3(1.f, 0.f, 0.f));
	mMat = glm::rotate(mMat, toRadians(35.f), glm::vec3(0.f, 1.f, 0.f));
	mMat = glm::rotate(mMat, toRadians(35.f), glm::vec3(0.f, 0.f, 1.f));

	//这句必须要有，否则鼠标中建失效
	pMat = glm::perspective(camera.Zoom, aspect, 0.01f, 1000.f);

	mvMat = vMat * mMat;

	//更改一个uniform矩阵变量或数组的值。要更改的uniform变量的位置由location指定，location的值应该由glGetUniformLocation函数返回
	// 将透视矩阵和MV 矩阵复制给相应的统一变量
	/*通过一致变量（uniform修饰的变量）引用将一致变量值传入渲染管线。
		location : uniform的位置。
		count : 需要加载数据的数组元素的数量或者需要修改的矩阵的数量。
		transpose : 指明矩阵是列优先(column major)矩阵（GL_FALSE）还是行优先(row major)矩阵（GL_TRUE）。
		value : 指向由count个元素的数组的指针。
	*/
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));  //GL_FALSE 参数不能错否则无法显示obj模型

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shuttleTextureId);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	//指定了渲染时索引值为 index 的顶点属性数组的数据格式和位置
	/*Parameters
	index
		指定要修改的顶点属性的索引值

		size
		指定每个顶点属性的组件数量。必须为1、2、3或者4。初始值为4。（梦维：如position是由3个（x, y, z）组成，而颜色是4个（r, g, b, a））

		type
		指定数组中每个组件的数据类型。可用的符号常量有GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_FIXED, 和 GL_FLOAT，初始值为GL_FLOAT。

		normalized
		指定当被访问时，固定点数据值是否应该被归一化（GL_TRUE）或者直接转换为固定点值（GL_FALSE）。

		stride
		指定连续顶点属性之间的偏移量。如果为0，那么顶点属性会被理解为：它们是紧密排列在一起的。初始值为0。

		pointer
		指定一个指针，指向数组中第一个顶点属性的第一个组件。初始值为0。
		*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//如果启用，将访问通用顶点属性数组中的值，并在调用顶点数组命令（如glDrawArrays或glDrawElements）时用于呈现。
	//对应顶点着色器的layout(location=0) in vec3 position;
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shuttleTextureId);

	glEnable(GL_DEPTH_TEST);
	//指定用于深度缓冲比较值；
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, myModel.getNumVertices());
}


void press_key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if ((key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS))
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	if (action == GLFW_PRESS)
	{
		keys[key] = GLFW_TRUE;
	}
	else if (action == GLFW_RELEASE)
	{
		keys[key] = GLFW_FALSE;
	}
}

void mouse_move_callback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastLocX = xPos;
		lastLocY = yPos;
		firstMouse = GL_FALSE;
	}

	double offsetLocX = xPos - lastLocX;
	double offsetLocY = lastLocY - yPos;

	lastLocX = xPos;
	lastLocY = yPos;

	camera.ProcessMouseMovement(offsetLocX, offsetLocY);
}

void mouse_scroll_callback(GLFWwindow* window, double xPos, double yPos)
{
	camera.ProcessMouseScroll(yPos);
}



int main(int argc, char** argv)
{
	int glfwState = glfwInit();
	if (GLFW_FALSE == glfwState)
	{
		cout << "GLFW initialize failed,invoke glfwInit()......Error file:" << __FILE__ << "......Error line:" << __LINE__ << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	/*因为我们要使用OpenGL 4.6，所以我们把GLFW_CONTEXT_VERSION_MAJOR和GLFW_CONTEXT_VERSION_MINOR对应的hint都设置为4和6。
	因为我们要使用OpenGL核心模式（这个后面会提到更多），所以我们把GLFW_OPENGL_PROFILE对应的hint设置为GLFW_OPENGL_CORE_PROFILE，
	表示使用OpenGL核心模式。最后，把GLFW_RESIZABLE对应的hint设置为GLFW_FALSE，表示窗口不允许用户调整大小。
	之所以这样做是因为如果允许用户调整大小，大小发生变化后，窗口的绘制区域默认不变（依然是原来窗口的区域），
	也就是说窗口上绘制的图像的大小、位置不会发生改变。为了避免这种现象发生，我们就简单地不让用户调整窗口大小
	（当然也有更好的方法，就是用GLFW设置一个窗口大小的回调函数，但这样比较简单）。*/
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GLFW_OPENGL_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "Load obj file model", nullptr, nullptr);
	if (!window)
	{
		cout << "GLFW create window failed,invoke glfwCreateWindow()......Error line:" << __FILE__ << "......Error line:" << __LINE__ << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	/*此函数使调用线程上的指定窗口的 OpenGL 或 OpenGL ES 上下文成为当前上下文。
	  一次只能在单个线程上使上下文成为当前上下文，并且每个线程一次只能有一个当前上下文。
	  在线程之间移动上下文时，必须先使其在旧线程上变为非当前状态，然后再在新线程上变为当前状态。
	*/
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, press_key_callback);
	glfwSetCursorPosCallback(window, mouse_move_callback);
	glfwSetScrollCallback(window, mouse_scroll_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);

	//设置鼠标模式
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	int glewState = glewInit();
	if (GLEW_OK != glewState)
	{
		cout << "GLEW initialize failed,invoke glewInit()......Error file:" << __FILE__ << "......Error line:" << __LINE__ << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	/*此函数设置当前 OpenGL 或 OpenGL ES 上下文的交换间隔，即从调用glfwSwapBuffers开始等待的屏幕更新次数，
	  然后再交换缓冲区并返回。这有时称为垂直同步、垂直回扫同步或仅vsync。
	  支持WGL_EXT_swap_control_tear和GLX_EXT_swap_control_tear扩展的上下文也接受负交换间隔，这允许驱动程序立即交换，
	  即使帧到达有点晚。您可以使用glfwExtensionSupported检查这些扩展。
	  上下文必须在调用线程上是最新的。在没有当前上下文的情况下调用此函数将导致GLFW_NO_CURRENT_CONTEXT错误。
	  此功能不适用于 Vulkan。如果您使用 Vulkan 进行渲染，请改为查看交换链的当前模式。
	*/
	glfwSwapInterval(1);

	printf("%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));//开始初始化过程
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* version = glGetString(GL_VERSION);
	const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	printf("GL Vendor : %s\n", vendor);
	printf("GL Renderer : %s\n", renderer);
	printf("GL Version (string) : %s\n", version);
	printf("GL Version (integer) : %d.%d\n", major, minor);
	printf("GLSL Version : %s\n", glslVersion);

	glGetError(); // Debug GLEW bug fix


	/*因为我们要使用OpenGL 4.6，所以我们把GLFW_CONTEXT_VERSION_MAJOR和GLFW_CONTEXT_VERSION_MINOR对应的hint都设置为4和6。
	因为我们要使用OpenGL核心模式（这个后面会提到更多），所以我们把GLFW_OPENGL_PROFILE对应的hint设置为GLFW_OPENGL_CORE_PROFILE，
	表示使用OpenGL核心模式。最后，把GLFW_RESIZABLE对应的hint设置为GLFW_FALSE，表示窗口不允许用户调整大小。
	之所以这样做是因为如果允许用户调整大小，大小发生变化后，窗口的绘制区域默认不变（依然是原来窗口的区域），
	也就是说窗口上绘制的图像的大小、位置不会发生改变。为了避免这种现象发生，我们就简单地不让用户调整窗口大小
	（当然也有更好的方法，就是用GLFW设置一个窗口大小的回调函数，但这样比较简单）。*/



	/*默认情况下，出于性能考虑，所有顶点着色器的属性（Attribute）变量都是关闭的，
	  意味着数据在着色器端是不可见的，哪怕数据已经上传到GPU，由glEnableVertexAttribArray启用指定属性，
	  才可在顶点着色器中访问逐顶点的属性数据。glVertexAttribPointer或VBO只是建立CPU和GPU之间的逻辑连接，
	  从而实现了CPU数据上传至GPU。但是，数据在GPU端是否可见，即，着色器能否读取到数据，由是否启用了对应的属性决定，
	  这就是glEnableVertexAttribArray的功能，允许顶点着色器读取GPU（服务器端）数据。
	 */
	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

	init(window);

	while (!glfwWindowShouldClose(window))
	{
		display(window, (float)glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);

	return 0;
}