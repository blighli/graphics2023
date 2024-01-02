#include "Shader.h"
#include "Primative.h"
#include "Texture.h"
#include "Camera.h"
#include "Light.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <direct.h>

// Called when window size changed.
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;

const unsigned int SCR_HEIGHT = 600;

// timing
float delta_time = 0.0f;	// time between current frame and last frame

float last_frame = 0.0f;

auto camera = core::scene::Camera{ glm::vec3{0.0f, 0.0f, 3.0f} };

// mouse
bool first_mouse = true;

float last_x = 0.0f, last_y = 0.0f;

int main()
{
	// init
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "hw2", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// main
	auto shader = core::shader::Shader("camera_texture_vertex.glsl", "camera_texture_fragment.glsl");
	auto triangle_1 = core::primative::Triangle{
		{0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{1.0f, 1.0f, 0.0f},
		{1.0f, 1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f, 1.0f},
		{0.0f, 1.0f},
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f}
	};

	auto sphere_1 = core::primative::Sphere{ {-1.0f, 0.0f, -1.0f}, 0.5f, 50, 50 };
	auto sphere_2 = core::primative::Sphere{ {1.0f, 1.0f, -1.0f}, 0.3f, 50, 50 };
	auto sphere_3 = core::primative::Sphere{ {1.0f, -1.0f, -1.0f}, 0.4f, 50, 50 };

	auto manager = core::resource::Text_Manager::instance();
	manager->load_texture("keqing", "keqing6.png");
	manager->load_texture("ganyu", "ganyu1.png");
	manager->load_texture("furina", "furina1.jpg");
	auto tex_keqing = manager->get_texture("keqing");

	//std::cout << tex_keqing->channels << std::endl;
	triangle_1.set_tex("keqing");
	sphere_1.set_tex("keqing");
	sphere_2.set_tex("ganyu");
	sphere_3.set_tex("furina");
	sphere_1.name = "keqing";
	sphere_1.sheld_color = glm::vec3(0.8f, 0.4f, 0.8f);

	sphere_2.name = "ganyu";
	sphere_2.sheld_color = glm::vec3(0.5f, 0.5f, 1.0f);

	sphere_3.name = "furina";
	sphere_3.sheld_color = glm::vec3(0.5f, 1.0f, 0.5f);

	//light
	auto light1 = core::light::Point_Light{ {1.0f, 1.0f, 1.0f}, { 1.0f, 3.0f, 3.0f} };


	// main loop
	while (!glfwWindowShouldClose(window)) {
		//time
		float current_frame = static_cast<float>(glfwGetTime());
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		//std::cout << last_x << "," << last_y << std::endl;

		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		
		auto projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
		
		//
		auto height = 0.1f * std::tanf(glm::radians(camera.Zoom * 0.5f)) * 2;
		auto x_in_scene = (last_x - static_cast<float>(SCR_WIDTH) * 0.5f)  * height / SCR_HEIGHT;
		auto y_in_scene = (static_cast<float>(SCR_HEIGHT) * 0.5f - last_y) * height / SCR_HEIGHT;
		auto direction = glm::normalize(camera.Right * x_in_scene + camera.Up * y_in_scene + camera.Front * 0.1f);
		core::control::Ray ray{ camera.Position, direction };
		//
		
		auto view = camera.GetViewMatrix();
		auto model = glm::mat4{ 1.0f };
		shader.set_mat4("model", model);
		shader.set_mat4("view", view);
		shader.set_mat4("projection", projection);
		shader.set_vec3("light_pos", light1.position);
		shader.set_vec3("light_color", light1.color);
		shader.set_vec3("view_pos", camera.Position);
		
		shader.use();
		shader.set_int("texture1", 0);
		//triangle_1.draw();

		// sphere_1
		if (sphere_1.intersect_mesh(ray)) {
			shader.set_vec3("sheld_color", sphere_1.sheld_color);
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) {
				std::cout << sphere_1.name << " clicked\n";
			}
		}
		else {
			shader.set_vec3("sheld_color", glm::vec3{1.0f, 1.0f, 1.0f});
		}
		sphere_1.draw();

		// sphere_2
		if (sphere_2.intersect_mesh(ray)) {
			shader.set_vec3("sheld_color", sphere_2.sheld_color);
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) {
				std::cout << sphere_2.name << " clicked\n";
			}
		}
		else {
			shader.set_vec3("sheld_color", glm::vec3{ 1.0f, 1.0f, 1.0f });
		}
		sphere_2.draw();

		// sphere_3
		if (sphere_3.intersect_mesh(ray)) {
			shader.set_vec3("sheld_color", sphere_3.sheld_color);
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) {
				std::cout << sphere_3.name << " clicked\n";
			}
		}
		else {
			shader.set_vec3("sheld_color", glm::vec3{ 1.0f, 1.0f, 1.0f });
		}
		sphere_3.draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(core::scene::Camera_Movement::FORWARD, delta_time);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(core::scene::Camera_Movement::BACKWARD, delta_time);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(core::scene::Camera_Movement::LEFT, delta_time);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(core::scene::Camera_Movement::RIGHT, delta_time);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(core::scene::Camera_Movement::UP, delta_time);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		camera.ProcessKeyboard(core::scene::Camera_Movement::DOWN, delta_time);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (first_mouse)
	{
		last_x = xpos;
		last_y = ypos;
		first_mouse = false;
	}

	float xoffset = xpos - last_x;
	float yoffset = last_y - ypos; // reversed since y-coordinates go from bottom to top

	last_x = xpos;
	last_y = ypos;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}
