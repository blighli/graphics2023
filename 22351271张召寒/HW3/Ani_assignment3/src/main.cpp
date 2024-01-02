#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>
#include <model.h>

#include <iostream>
#include <filesystem>
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 1.5f, 1.2f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//以模型为中心的平移旋转缩放
//平移
glm::vec3 translateChange(0.f);
float translateSpeed = 0.05f;
//旋转
glm::vec3 rotateAxis(0.f,1.0f,0.f);
float rotateAngle = 0.0f;
float rotateSpeed = 1.0f;
//缩放
glm::vec3 scaleFactor(1.0f, 1.0f, 1.0f);
glm::vec3 scaleSpeed(0.05f, 0.05f, 0.05f);
int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------

	std::filesystem::path currentPath = std::filesystem::current_path();
	std::cout << "Current path: " << currentPath << std::endl;

	Shader modelShader("shader/shader.vs", "shader/shader.fs");

	// load models
	// -----------
	Model ourModel("model/nanosuit/nanosuit.obj");

	//多光源位置
	glm::vec3 pointLightPositions[] = {
	glm::vec3(0.8f,  0.2f,  2.0f),
	glm::vec3(2.3f, -4.3f, -5.0f),
	glm::vec3(-5.0f,  2.0f, -10.0f),
	glm::vec3(0.0f,  1.0f, -3.0f)
	};


	glm::mat4 model, view, projection;


	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		//ourShader.use();
		modelShader.use();

		//设置多光源shader
		modelShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		modelShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		modelShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		modelShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		modelShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		modelShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		modelShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		modelShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		modelShader.setFloat("pointLights[0].constant", 1.0f);
		modelShader.setFloat("pointLights[0].linear", 0.09f);
		modelShader.setFloat("pointLights[0].quadratic", 0.032f);
		// point light 2
		modelShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		modelShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		modelShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		modelShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		modelShader.setFloat("pointLights[1].constant", 1.0f);
		modelShader.setFloat("pointLights[1].linear", 0.09f);
		modelShader.setFloat("pointLights[1].quadratic", 0.032f);
		// point light 3
		modelShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		modelShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		modelShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		modelShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		modelShader.setFloat("pointLights[2].constant", 1.0f);
		modelShader.setFloat("pointLights[2].linear", 0.09f);
		modelShader.setFloat("pointLights[2].quadratic", 0.032f);
		// point light 4
		modelShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		modelShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		modelShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		modelShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		modelShader.setFloat("pointLights[3].constant", 1.0f);
		modelShader.setFloat("pointLights[3].linear", 0.09f);
		modelShader.setFloat("pointLights[3].quadratic", 0.032f);
		// spotLight
		modelShader.setVec3("spotLight.position", camera.Position);
		modelShader.setVec3("spotLight.direction", camera.Front);
		modelShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		modelShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		modelShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		modelShader.setFloat("spotLight.constant", 1.0f);
		modelShader.setFloat("spotLight.linear", 0.09f);
		modelShader.setFloat("spotLight.quadratic", 0.032f);
		modelShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		modelShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		modelShader.setVec3("material.specular", 1.0f, 1.0f, 1.0f);
		modelShader.setFloat("material.shininess", 256.0f);

		modelShader.setVec3("viewPos", camera.Position);
		//std::cout <<"(" <<camera.Position.x<<","<< camera.Position.y<< "," << camera.Position.z<<")"<<endl;

		view = projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		view = camera.GetViewMatrix();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f) + translateChange); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(rotateAngle), rotateAxis);
		
		
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f)*scaleFactor);	// it's a bit too big for our scene, so scale it down
		modelShader.setMat4("model", model);
		modelShader.setMat4("view", view);
		modelShader.setMat4("projection", projection);
		ourModel.Draw(modelShader);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	//x
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS)
		translateChange.x += translateSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		translateChange.x -= translateSpeed;
	//y
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS)
		translateChange.y += translateSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		translateChange.y -= translateSpeed;
	//z
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS)
		translateChange.z += translateSpeed;
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		translateChange.z -= translateSpeed;

	//x
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS)
	{
		rotateAxis = glm::vec3(1.0f, 0.0f, 0.0f);
		rotateAngle += rotateSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		rotateAxis = glm::vec3(1.0f, 0.0f, 0.0f);
		rotateAngle -= rotateSpeed;
	}
	//y
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS)
	{
		rotateAxis = glm::vec3(0.0f, 1.0f, 0.0f);
		rotateAngle += rotateSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		rotateAxis = glm::vec3(0.0f, 1.0f, 0.0f);
		rotateAngle -= rotateSpeed;
	}
	//z
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS)
	{
		rotateAxis = glm::vec3(0.0f, 0.0f, 1.0f);
		rotateAngle += rotateSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		rotateAxis = glm::vec3(0.0f, 0.0f, 1.0f);
		rotateAngle -= rotateSpeed;
	}
	//bigger
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS)
		scaleFactor += scaleSpeed;
	//smaller
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		scaleFactor -= scaleSpeed;
		scaleFactor = glm::max(scaleFactor, glm::vec3(0.1f));
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}