#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <ctime>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
Camera camera(glm::vec3(0.0f, 0.0f, 36.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
const float rotate_angle_step_const = 0.5f;
float rotate_angle_step = rotate_angle_step_const;

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        std::cout << "Esc_close" << std::endl;
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        rotate_angle_step += 0.001f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        rotate_angle_step -= 0.001f;
    }
    float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        //std::cout << "W" << std::endl;
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        //std::cout << "S" << std::endl;
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        //std::cout << "A" << std::endl;
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        //std::cout << "D" << std::endl;
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        //std::cout << "J" << std::endl;
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        //std::cout << "K" << std::endl;
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        rotate_angle_step = rotate_angle_step_const;
    }
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrComponents;
    //stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

const GLfloat PI = 3.14159265358979323846f;
const unsigned int X_SEGMENTS = 60;
const unsigned int Y_SEGMENTS = 60;
std::vector<float> vertices;
std::vector<int> indices;
void sphere() {
    for (int y = 0; y <= Y_SEGMENTS; y++) {

        float yAngle = (float)y / (float)Y_SEGMENTS * PI;
        float yPos = std::cos(yAngle);//[1,-1]
        float yTex = (float)y / (float)Y_SEGMENTS;//[0,1]
        for (int x = 0; x <= X_SEGMENTS;x++) {
            float xAngle = (float)x / (float)X_SEGMENTS * 2.0f * PI;
            float xPos = std::cos(xAngle) * std::sin(yAngle);//[1,0],[0,-1],[-1,0][0,1] * std::sin(yAngle)
            float zPos = std::sin(xAngle) * std::sin(yAngle);//[0,1],[1,0],[0,-1],[-1,0] * std::sin(yAngle)
            float xTex = (float)x / (float)X_SEGMENTS;//[0,1]
            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);

            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);

            vertices.push_back(xTex);
            vertices.push_back(yTex);
        }
    }
    for (int y = 0; y < Y_SEGMENTS; y++) {
        for (int x = 0; x <= X_SEGMENTS;x++) {

            indices.push_back(y * (X_SEGMENTS + 1) + x);
            indices.push_back(y * (X_SEGMENTS + 1) + x + 1);
            indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            indices.push_back(y * (X_SEGMENTS + 1) + x + 1);
            indices.push_back((y + 1) * (X_SEGMENTS + 1) + x + 1);
            indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
        }
    }
}

int main() {
    //GLFE
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Project02", NULL, NULL);
    glfwMakeContextCurrent(window);
    //GLAD
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowAspectRatio(window, SCR_WIDTH, SCR_HEIGHT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    sphere();

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    Shader lightShader("light.vs", "light.fs");
    Shader lightingShader("lighting.vs", "lighting.fs");

    unsigned int sunTexture = loadTexture("sun.jpg");
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, sunTexture);
    unsigned int earthTexture = loadTexture("earth.jpg");
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, earthTexture);
    unsigned int moonTexture = loadTexture("moon.jpg");
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, moonTexture);

    glm::mat4 model, view, projection;
    glm::vec3 sunPosition(0.0f, 0.0f, 0.0f);

    float sun_d = 6.0f;//float sun_d = 1392000.0f;
    float sun_rotate = 25.0f * 24.0f;//float sun_rotate = 25.05f;
    float sun_earth_len = 18.0f;//float sun_earth_len = 149597870.0f;
    float earth_d = 2.0f;//float earth_d = 12742.0f;
    float earth_rotate = 1.0f * 24.0f;
    float earth_sun_rotate = 365.0f * 24.0f;//float earth_sun_rotate = 365.256363f;
    float moon_earth_len = 6.0f;//float moon_earth_len = 384000.0f;
    float moon_d = 0.5f;//float moon_d = 3476.28f;
    float moon_rotate = 27.0f * 24.0f;//float moon_rotate = 27.32f;
    float moon_earth_rotate = 27.0f * 24.0f;//float moon_earth_rotate = 27.32f;

    glm::vec3 sun_rotate_axis = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 earth_rotate_axis = glm::vec3(cos(glm::radians(66.34)), sin(glm::radians(66.34)), 0.0f);
    glm::vec3 moon_rotate_axis = glm::vec3(cos(glm::radians(1.5424)), sin(glm::radians(1.5424)), 0.0f);

    
    float rotated_angle = 0.0;
    //rotate_angle_step = 20.0f;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        rotated_angle += rotate_angle_step;

        processInput(window);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);
        
        // sun
        //glBindTexture(GL_TEXTURE_2D, sunTexture);
        lightShader.use();
        lightShader.setInt("light_texture", 0);
        //MVP
        model = glm::mat4(1.0f);
        model = glm::translate(model, sunPosition);
        model = glm::scale(model, glm::vec3(sun_d));
        model = glm::rotate(model, glm::radians(rotated_angle * earth_rotate / sun_rotate), sun_rotate_axis);
        lightShader.setMat4("model", model);
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        // earth
        //glBindTexture(GL_TEXTURE_2D, earthTexture);
        float earth_sun_angle = glm::radians(rotated_angle * earth_rotate / earth_sun_rotate);
        glm::vec3 earthPosition = glm::vec3(sun_earth_len * sin(earth_sun_angle), 0, sun_earth_len * cos(earth_sun_angle));//�������̫��λ��
        lightingShader.use();
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightPos", sunPosition);
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setInt("obj_texture", 1);
        //MVP
        model = glm::mat4(1.0f);
        model = glm::translate(model, earthPosition);
        model = glm::scale(model, glm::vec3(earth_d));
        model = glm::rotate(model, glm::radians(rotated_angle * earth_rotate /earth_rotate), earth_rotate_axis);
        lightingShader.setMat4("model", model);
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("projection", projection);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        // moon
        //glBindTexture(GL_TEXTURE_2D, moonTexture);
        float moon_earth_angle = glm::radians(rotated_angle * earth_rotate / moon_earth_rotate);
        glm::vec3 moon_earth_pos = glm::vec3(moon_earth_len * sin(moon_earth_angle), 0, moon_earth_len * cos(moon_earth_angle));//������Ե���λ��
        glm::vec3 moonPosition = earthPosition + moon_earth_pos;
        lightingShader.use();
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightPos", sunPosition);
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setInt("obj_texture", 2);
        //MVP
        model = glm::mat4(1.0f);
        model = glm::translate(model, moonPosition);
        model = glm::scale(model, glm::vec3(moon_d));
        model = glm::rotate(model, glm::radians(rotated_angle * earth_rotate / moon_rotate), moon_rotate_axis);
        lightingShader.setMat4("model", model);
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("projection", projection);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}