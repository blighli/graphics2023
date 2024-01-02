#include <string>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "Camera.h"
#include "model.h"
#include <windows.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

/*窗口初始设置*/
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

/*相机*/
Camera camera(glm::vec3(-3.0f, 0.0f, 20.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

/*着色器*/
Shader ctrl_shader;
Shader surface_shader;
Shader model_shader;
Shader vertex_shader;

/*计时（鼠标输入相关）*/
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//鼠标
bool mouse_left_button = false;
bool mouse_right_button = false;

/*变换相关*/
glm::mat4 model_surface = glm::scale(glm::mat4(1.0f), glm::vec3(1.2f, 1.2f, 1.2f));
glm::mat4 model_model = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(7.0f, 3.0f, 0.0f)), glm::vec3(0.01f, 0.01f, 0.01f));
float movement_speed_model = 4.0f;
float mouse_sensitivity_model = 0.2;
glm::mat4 rot = glm::mat4(1.0);

/*控制模型/曲面*/
bool control_model = true;
bool control_surface = false;

/*线框/顶点模式*/
bool wireMode = false;
bool vertexMode = false;
bool iswireMode = false;
bool isvertexMode = false;
/*曲面细分等级*/
float level = 200.0f;

int main() {
    /*-------------------初始化-------------------*/
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   
   
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Project2-3", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
	glewExperimental = GL_TRUE;
	glewInit();

    Model model_to_load("E:\\vs source\\repos\\Assignment2\\Assignment2\\model\\111.obj");
    

    ctrl_shader.setupShader("E:\\vs source\\repos\\Assignment2\\Assignment2\\shader\\surface.vs.glsl", "E:\\vs source\\repos\\Assignment2\\Assignment2\\shader\\vert.fs.glsl");
    surface_shader.setupShader("E:\\vs source\\repos\\Assignment2\\Assignment2\\shader\\surface.vs.glsl", "E:\\vs source\\repos\\Assignment2\\Assignment2\\shader\\surface.tcs.glsl",
                               "E:\\vs source\\repos\\Assignment2\\Assignment2\\shader\\surface.tes.glsl", "E:\\vs source\\repos\\Assignment2\\Assignment2\\shader\\texture.fs.glsl");
    model_shader.setupShader("E:\\vs source\\repos\\Assignment2\\Assignment2\\shader\\model_loading.vs.glsl", "E:\\vs source\\repos\\Assignment2\\Assignment2\\shader\\model_loading.fs.glsl");
    vertex_shader.setupShader("E:\\vs source\\repos\\Assignment2\\Assignment2\\shader\\model_loading.vs.glsl", "E:\\vs source\\repos\\Assignment2\\Assignment2\\shader\\vert.fs.glsl");


    float ctrl_points[] = {
        -2.,  2.,  1.2,
        -1.,  2.,  1.,
        0.,  2.,  0.5,
        1.,  2.,  1.,
        2.,  2.,  1.2,
        
        -2.,  1.,  0.,
        -1.,  1.,  0.,
        0.,  1.,  0.,
        1.,  1.,  0.,
        2.,  1.,  0.,
        
        -2.,  0.,  0.,
        -1.,  0.,  0.,
        0.,  0., -0.5,
        1.,  0.,  0.,
        2.,  0.,  0.,
        
        -2., -1.,  0.,
        -1., -1.,  0.,
        0., -1.,  0.,
        1., -1.,  0.,
        2., -1.,  0.,
        
        -2., -2.,  1.2,
        -1., -2.,  1.,
        0., -2.,  0.5,
        1., -2.,  1.,
        2., -2.,  1.2
    };
    

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ctrl_points), ctrl_points, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    

    unsigned int texture;
    glGenTextures(1, &texture);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int width, height, num_channels;

    unsigned char* data = stbi_load("E:\\vs source\\repos\\Assignment2\\Assignment2\\texture\\111.jpg", &width, &height, &num_channels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
    }
    else {
        std::cout << "Failed to load texture." << std::endl;
    }
    surface_shader.use();
    surface_shader.setInt("Texture", 0);
    stbi_image_free(data);
    
    glEnable(GL_DEPTH_TEST);
    

    while (!glfwWindowShouldClose(window)) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        
        
        glClearColor(0.059f, 0.816f, 0.78f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        
     
        surface_shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        surface_shader.setMat4("inverse", glm::inverse(projection));
        surface_shader.setMat4("frag_view", view);
        surface_shader.setMat4("frag_model", model_surface);
        surface_shader.setVec3("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));
        surface_shader.setVec3("viewPos", camera.Position);
        

        surface_shader.setVec3("material.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
        surface_shader.setVec3("material.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        surface_shader.setVec3("material.specular", glm::vec3(0.6f, 0.6f, 0.6f));
        surface_shader.setFloat("material.shininess", 128.0f);
        
        if (!vertexMode) {
       
            glBindVertexArray(VAO);
            surface_shader.use();
  
            surface_shader.setFloat("outer_x", level);
            surface_shader.setFloat("outer_y", level);
            surface_shader.setFloat("inner_vert", level);
            surface_shader.setFloat("inner_hori", level);
            // ±‰ªªæÿ’Û
            surface_shader.setMat4("projection", projection);
            surface_shader.setMat4("view", view);
            surface_shader.setMat4("model", model_surface);
            // Œ∆¿Ì
            surface_shader.setInt("Texture", 0);
            
            glPolygonMode(GL_FRONT_AND_BACK, wireMode ? GL_LINE : GL_FILL);
            
            glPatchParameteri(GL_PATCH_VERTICES, 25);
            glDrawArrays(GL_PATCHES, 0, 25);
            glBindVertexArray(0);
        }
        else {
            // ªÊ÷∆øÿ÷∆µ„
            glBindVertexArray(VAO);
            ctrl_shader.use();
            ctrl_shader.setMat4("projection", projection);
            ctrl_shader.setMat4("view", view);
            ctrl_shader.setMat4("model", model_surface);
            glPointSize(10.0f);
            glDrawArrays(GL_POINTS, 0, 25);
            glBindVertexArray(0);
        }
        
        if (!vertexMode) {
            // ªÊ÷∆ƒ£–Õ
            model_shader.use();
            model_shader.setMat4("projection", projection);
            model_shader.setMat4("view", view);
            model_shader.setMat4("model", model_model);
            
            // ƒ£–Õπ‚’’
            model_shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
            model_shader.setMat4("inverse", glm::inverse(projection));
            model_shader.setMat4("fragView", view);
            model_shader.setVec3("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));
            model_shader.setVec3("viewPos", camera.Position);
            
            model_to_load.Draw(model_shader, vertexMode);
        }
        else {
            // ªÊ÷∆ƒ£–Õ∂•µ„
            vertex_shader.use();
            vertex_shader.setMat4("projection", projection);
            vertex_shader.setMat4("view", view);
            vertex_shader.setMat4("model", model_model);
            
            model_to_load.Draw(vertex_shader, vertexMode);
        }
        processInput(window);
        // Ωªªªª∫≥Â°¢ºÏ≤È ¬º˛¥•∑¢◊¥Ã¨º∞∏¸–¬¥∞ø⁄◊¥Ã¨
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
    //  Õ∑≈◊ ‘¥
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (mouse_right_button) {
        float xoffset_c = xpos - lastX;
        float yoffset_c = lastY - ypos;
        lastX = xpos;
        lastY = ypos;
        camera.ProcessMouseMovement(xoffset_c, yoffset_c);
    }
    if (mouse_left_button && control_model) {
        float xoffset_m = xpos - lastX;
        float yoffset_m = lastY - ypos;;
        lastX = xpos;
        lastY = ypos;
        float radians_x = xoffset_m * mouse_sensitivity_model;
        float radians_y = -yoffset_m * mouse_sensitivity_model;
        if (radians_y > 89.0f) {
            radians_y = 89.0f;
        }
        if (radians_y < -89.0f) {
            radians_y = -89.0f;
        }
        rot = glm::mat4(1.0f);
        rot = glm::rotate(rot, glm::radians(radians_x), glm::vec3(0.0f, 1.0f, 0.0f));
        rot = glm::rotate(rot, glm::radians(radians_y), glm::vec3(1.0f, 0.0f, 0.0f));
        model_model = model_model * rot;
    }
    if (mouse_left_button && control_surface) {
        float xoffset_m = xpos - lastX;
        float yoffset_m = lastY - ypos;;
        lastX = xpos;
        lastY = ypos;
        float radians_x = xoffset_m * mouse_sensitivity_model;
        float radians_y = -yoffset_m * mouse_sensitivity_model;
        if (radians_y > 89.0f) {
            radians_y = 89.0f;
        }
        if (radians_y < -89.0f) {
            radians_y = -89.0f;
        }
        glm::mat4 rot_surface = glm::mat4(1.0f);
        rot_surface = glm::rotate(rot_surface, glm::radians(radians_x), glm::vec3(0.0f, 1.0f, 0.0f));
        rot_surface = glm::rotate(rot_surface, glm::radians(radians_y), glm::vec3(1.0f, 0.0f, 0.0f));
        model_surface = model_surface * rot_surface;
    }
    lastX = xpos;
    lastY = ypos;
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (button) {
            case GLFW_MOUSE_BUTTON_LEFT:
                mouse_left_button = true;
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                mouse_right_button = true;
                break;
            default:
                return;
        }
    }
    else if (action == GLFW_RELEASE) {
        switch (button) {
            case GLFW_MOUSE_BUTTON_LEFT:
                mouse_left_button = false;
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                mouse_right_button = false;
                break;
            default:
                return;
        }
    }
}
//模型缩放
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow* window) {
    // πÿ±’¥∞ø⁄
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    
    // œ‡ª˙Œª÷√
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        camera.ProcessKeyBoard(Camera_Movement::FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        camera.ProcessKeyBoard(Camera_Movement::BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        camera.ProcessKeyBoard(Camera_Movement::LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        camera.ProcessKeyBoard(Camera_Movement::RIGHT, deltaTime);
    }
    
    // ƒ£–Õ“∆∂Ø
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        model_model = glm::translate(model_model, glm::vec3(0.0f, movement_speed_model * deltaTime * 2.5f, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        model_model = glm::translate(model_model, glm::vec3(0.0f, -(movement_speed_model * deltaTime * 2.5f), 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        model_model = glm::translate(model_model, glm::vec3(-(movement_speed_model * deltaTime * 2.5f), 0.0f, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        model_model = glm::translate(model_model, glm::vec3(movement_speed_model * deltaTime * 2.5f, 0.0f, 0.0f));
    }
    
    // «˙√Ê“∆∂Ø
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        model_surface = glm::translate(model_surface, glm::vec3(0.0f, movement_speed_model * deltaTime, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        model_surface = glm::translate(model_surface, glm::vec3(0.0f, -(movement_speed_model * deltaTime), 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        model_surface = glm::translate(model_surface, glm::vec3(-(movement_speed_model * deltaTime), 0.0f, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        model_surface = glm::translate(model_surface, glm::vec3(movement_speed_model * deltaTime, 0.0f, 0.0f));
    }
    
    // œﬂøÚƒ£ Ω
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        if (!iswireMode) {
            wireMode = true;
            iswireMode = true;
        }
            
    }
    else {
        if (iswireMode) {
            wireMode = false;
            iswireMode = false;
        }
    }
    
    // ∂•µ„ƒ£ Ω
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        vertexMode = true;
    }
    else {
        //Sleep(2000);
        vertexMode = false;
    }
    
    // øÿ÷∆∂‘œÛ
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        control_model = true;
        control_surface = false;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
        control_model = false;
        control_surface = true;
    }
}
