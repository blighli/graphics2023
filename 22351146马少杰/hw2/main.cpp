#include <bits/stdc++.h>
#include "camera.h"
#include "utils.h"
#include "shader.h"
#include "texture.h"
#include "planet.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>
#include "raypicler.h"

using namespace std;
using namespace glm;

vec2 mousePos;
bool mousePressed = false;
bool motion = true;

int mode = 1;

Camera cam;
float speed = 0.5;
GLFWwindow* window = 0;

struct VAO {
	enum { GEOMETRY = 0 };
};

struct VBO {
	enum { POINTS = 0, NORMALS, UVS, INDICES };
};

GLuint vbo[4];
GLuint vao[1];
GLuint shader;
vector<vec3> points;
vector<vec2> uvs;
Planet sun, mercury, venus, earth, mars, jupiter, saturn, uranus, neptune, pluto, moon, skybox;
vector<Planet*> planets;

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_W) {
		cam.moveCamera(0.0, -1.0, 0.0);
	}

	if (key == GLFW_KEY_S) {
		cam.moveCamera(0.0, 1.0, 0.0);
	}

	if (key == GLFW_KEY_A) {
		cam.moveCamera(1.0, 0.0, 0.0);
	}

	if (key == GLFW_KEY_D) {
		cam.moveCamera(-1.0, 0.0, 0.0);
	}

	if (key == GLFW_KEY_1) {
		mode = 1;
		cam.polarPos.z = 50.0;
	}
	if (key == GLFW_KEY_2) {
		mode = 2;
		cam.polarPos.z = 10.0;
	}
	if (key == GLFW_KEY_3) {
		mode = 3;
		cam.polarPos.z = 5.0;
	}
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if ((action == GLFW_PRESS) || (action == GLFW_RELEASE))
		mousePressed = !mousePressed;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		int screenWidth, screenHeight;
		glfwGetWindowSize(window, &screenWidth, &screenHeight);

		Ray ray = getRayFromMouse(static_cast<int>(mouseX), static_cast<int>(mouseY), screenWidth, screenHeight, cam);

		float t;
		for (const Planet* planet : planets) {
			if (rayIntersectsSphere(ray, *planet, t)) {
				if (planet->name != "skybox") {
					printf("Selected Planet: %s\n", planet->name.c_str());
				}
			}
		}
	}
}

void mousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	vec2 newPos = vec2(xpos / (double)vp[2], -ypos / (double)vp[3]) * 2.f - vec2(1.f);

	vec2 diff = newPos - mousePos;
	if (mousePressed)
		cam.translateCamera(-diff.y, diff.x, 0.0);

	mousePos = newPos;
}

void resizeCallback(GLFWwindow* window, int width, int height)
{
	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	float aspectRatio = (float)vp[2] / (float)vp[3];

	if (width > height) {
		int newWidth = height * aspectRatio;
		glViewport((width - newWidth) / 2, 0, newWidth, height);
	}
	else {
		int newHeight = width / aspectRatio;
		glViewport(0, (height - newHeight) / 2, width, newHeight);
	}
}


void mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
	cam.translateCamera(0.0, 0.0, -yOffset * 50);
}

void generateIDs()
{
	glGenVertexArrays(1, vao);
	glGenBuffers(4, vbo);
}

void deleteIDs()
{
	glDeleteProgram(shader);
	glDeleteVertexArrays(1, vao);
	glDeleteBuffers(4, vbo);
}


void initVAO()
{
	glBindVertexArray(vao[VAO::GEOMETRY]);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::POINTS]);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vec3),
		(void*)0
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::NORMALS]);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vec3),
		(void*)0
	);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::UVS]);
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vec2),
		(void*)0
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[VBO::INDICES]);

	assert(glGetError() == GL_NO_ERROR && "initVAO error");
}

void loadBuffer(Planet* p)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::POINTS]);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(vec3) * p->positions.size(),
		&p->positions[0],
		GL_DYNAMIC_DRAW

	);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::NORMALS]);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(vec3) * p->normals.size(),
		&p->normals[0],
		GL_DYNAMIC_DRAW

	);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::UVS]);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(vec2) * p->uvs.size(),
		&p->uvs[0],
		GL_STATIC_DRAW
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[VBO::INDICES]);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		sizeof(unsigned int) * p->indices.size(),
		&p->indices[0],
		GL_STATIC_DRAW
	);

	assert(glGetError() == GL_NO_ERROR && "loadBuffer error");
}

void initGL()
{
	generateIDs();
	shader = createShader();
	initVAO();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

void render(Camera* cam, mat4 perspectiveMatrix, mat4 modelview, int startElement, int numElements)
{

	glUseProgram(shader);
	glBindVertexArray(vao[VAO::GEOMETRY]);
	glUseProgram(shader);
	mat4 camMatrix = cam->getMatrix();

	glUniformMatrix4fv(glGetUniformLocation(shader, "cameraMatrix"),
		1,
		false,
		&camMatrix[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(shader, "perspectiveMatrix"),
		1,
		false,
		&perspectiveMatrix[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(shader, "modelviewMatrix"),
		1,
		false,
		&modelview[0][0]);

	CheckGLErrors("loadUniforms");

	glDrawElements(
		GL_TRIANGLES,
		numElements,
		GL_UNSIGNED_INT,
		(void*)0
	);

	CheckGLErrors("render");
}

void update() {
	GLuint diffUniformLocation;
	mat4 perspectiveMatrix = perspective(radians(80.f), 1.f, 0.1f, 1000.f);

	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (size_t i = 0; i < planets.size(); i++) {
		if (planets[i]->name != "moon") {
			planets[i]->orbitPlanet(sun.center, vec3(0.0, 0.0, 1.0), planets[i]->orb);
			planets[i]->rotatePlanet(vec3(0.0, 0.0, 1.0), planets[i]->rot);
		}
		else {
			planets[i]->orbitPlanet(earth.center, vec3(0.0, 0.0, 1.0), planets[i]->orb);
			planets[i]->rotatePlanet(vec3(0.0, 0.0, 1.0), planets[i]->rot);
		}
	}

	glUseProgram(shader);

	for (size_t i = 0; i < planets.size(); i++) {
		loadBuffer(planets[i]);
		loadTexture(planets[i]->texture, GL_TEXTURE0, shader, string("texSphere"));
		diffUniformLocation = glGetUniformLocation(shader, "diffuse");
		if (planets[i]->light)
			glUniform1i(diffUniformLocation, 0);
		else
			glUniform1i(diffUniformLocation, 1);
		render(&cam, perspectiveMatrix, mat4(1.f), 0, planets[i]->indices.size());
	}

	glfwSwapBuffers(window);

	glfwPollEvents();
}

int main(int argc, char* argv[])
{
	glfwInit();
	glfwSetErrorCallback(glfw_error_callback);
	assert(glGetError() == GL_NO_ERROR && "Failed to init GLFW");

	window = glfwCreateWindow(1024, 1024, "Solar system 22351146", 0, 0);
	assert(window && "Failed to create window");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, mousePosCallback);
	glfwSetScrollCallback(window, mouseScrollCallback);
	glfwSetWindowSizeCallback(window, resizeCallback);
	glfwMakeContextCurrent(window);

	initGL();

	float scale = speed * PI;

	/*
	Sun	Sun	0	696000	25.38	0	0
	Mercury	Sun	57909175	2439.7	58.646	87.97	0.20563069
	Venus	Sun	108208930	6051.8	243	224.7	0.0068
	Earth	Sun	149597890	6378.14	0.99726968	365.24	0.01671022
	Mars	Sun	227936640	3397	1.026	686.93	0.0934
	Jupiter	Sun	778412020	71492	0.41354	4330.46806	0.04839
	saturn	Sun	1426725400	60268	0.44401	10755.58752	0.0541506
	Uranus	Sun	2870972200	25559	0.718	30687.4648	0.047168
	Neptune	Sun	4498252900	24764	0.67125	60187.8996	0.00859
	Pluto	Sun	5906380000	1151	6.387	90550.3008	0.2488
	Moon	Earth	7687980	868.55	27.32	27.32	0.01
	不直观 不能使用真实数据
	*/

	sun = Planet(vec3(0.0), 35.0, 96, 1 / 25.38, 0.0);
	sun.generateSphere();
	sun.texture = createTexture(string("resources/sun.jpg"));
	sun.light = true;
	sun.name = "sun";

	mercury = Planet(vec3(107.0, 0.0, 0.0), 3.0, 64, scale / 58.646, scale / 58.646);
	mercury.generateSphere();
	mercury.texture = createTexture(string("resources/mercury.jpg"));
	mercury.name = "mercury";

	venus = Planet(vec3(129.0, 0.0, 0.0), 4.0, 64, scale / 243, scale / 224.7);
	venus.generateSphere();
	venus.texture = createTexture(string("resources/venus.jpg"));
	venus.name = "venus";

	earth = Planet(vec3(142.0, 0.0, 0.0), 4.0, 64, -scale, scale / 365);
	earth.generateSphere();
	earth.texture = createTexture(string("resources/earth.jpg"));
	earth.name = "earth";

	mars = Planet(vec3(161.0, 0.0, 0.0), 3.0, 96, scale / 1.026, scale / 686.93);
	mars.generateSphere();
	mars.texture = createTexture(string("resources/mars.jpg"));
	mars.name = "mars";

	jupiter = Planet(vec3(233.0, 0.0, 0.0), 9.0, 96, scale / 0.41354, scale / 4330.46806);
	jupiter.generateSphere();
	jupiter.texture = createTexture(string("resources/jupiter.jpg"));
	jupiter.name = "jupiter";

	saturn = Planet(vec3(279.0, 0.0, 0.0), 8.0, 96, scale / 0.44401, scale / 10755.58752);
	saturn.generateSphere();
	saturn.texture = createTexture(string("resources/saturn.jpg"));
	saturn.name = "saturn";

	uranus = Planet(vec3(344.0, 0.0, 0.0), 7.0, 96, scale / 0.718, scale / 30687.4648);
	uranus.generateSphere();
	uranus.texture = createTexture(string("resources/uranus.jpg"));
	uranus.name = "uranus";

	neptune = Planet(vec3(394.0, 0.0, 0.0), 7.0, 96, scale / 0.67125, scale / 60187.8996);
	neptune.generateSphere();
	neptune.texture = createTexture(string("resources/neptune.jpg"));
	neptune.name = "neptune";

	moon = Planet(vec3(132, 0.0, 0.0), 2.0, 48, scale / 27.32, scale / 27.32);
	moon.generateSphere();
	moon.texture = createTexture(string("resources/moon.jpg"));
	moon.name = "moon";

	skybox = Planet(vec3(0.0), 1000.0, 128, scale / 5000, 0.0);
	skybox.generateSphere();
	skybox.texture = createTexture(string("resources/space1.png"));
	skybox.name = "skybox";

	planets.push_back(&sun);
	planets.push_back(&mercury);
	planets.push_back(&venus);
	planets.push_back(&earth);
	planets.push_back(&mars);
	planets.push_back(&jupiter);
	planets.push_back(&saturn);
	planets.push_back(&uranus);
	planets.push_back(&neptune);
	planets.push_back(&pluto);
	planets.push_back(&moon);
	planets.push_back(&skybox);

	cam = Camera(vec3(-2.6, -2.6, 500.0), vec3(0.0, 0.0, 0.0), sun.radius);

	while (!glfwWindowShouldClose(window)) {
		update();
	}

	deleteIDs();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
