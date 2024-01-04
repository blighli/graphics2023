#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;
using namespace glm;

unsigned int WIDTH = 1024;
unsigned int HEIGHT = 800;

struct PointLight
{
    vec3 pose;
    vec3 intensity;
    float a0;
    float a1;
    float a2;
}pointLight = { vec3(-1.0f, 1.0f, 6.0f),vec3(1.0f,1.0f,1.0f),0.01f,0.09f,0.032f };

struct DirectionalLight
{
    vec3 direction;
    vec3 intensity;
}dirLight = { vec3(0.0,0.0,-1.0),vec3(1.0,1.0,1.0) };

struct SpotLight
{
    vec3 pos;
    vec3 direction;
    vec3 intensity;
    float alpha;
    float cutoff;
    float a0;
    float a1;
    float a2;
}spotLight = { vec3(0.0,0.0,10.0),vec3(0.0f,0.0f,-1.0f),vec3(1.0f,1.0f,1.0f),128.0f,radians(25.0f),0.05f,0.08f,0.002f };

struct Material
{
    float ka, kd, ks;
    float shiness;
}material = { 0.5f, 0.6f, 1.0f, 80.0f };
//0.1f, 0.5f, 0.8f, 64.0f
//0.8f, 0.2f, 0.8f, 20.0f
//0.5f, 0.6f, 1.0f, 80.0f
//0.4f, 1.0f, 0.2f, 50.0f

vec3 viewpos = vec3(2.0f, 2.0f, 8.0f);
mat4 model = mat4(1.0f);
mat4 view = lookAt(viewpos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
mat4 projection = perspective(80.0f, float(WIDTH) / float(HEIGHT), 0.1f, 100.0f);
const unsigned int DEPTH = 6;
const unsigned int MAXNUM = 8000000;
const unsigned int times = 1800;
const float Pi = 3.141592653f;
const float len = 500.0f;
const mat4 standard = mat4(1.0f);

unsigned int num = 0;
float vertices[MAXNUM] = { 0 };

void triangle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float offset, float R, float G, float B, int flag)
{
    vertices[num] = x1 + offset;
    num++;
    vertices[num] = y1;
    num++;
    vertices[num] = z1;
    num++;
    vertices[num] = R;
    num++;
    vertices[num] = G;
    num++;
    vertices[num] = B;
    num++;
    vertices[num] = flag ? (0 - x1) : 0;
    num++;
    vertices[num] = flag ? (0 - y1) : 1;
    num++;
    vertices[num] = 0;
    num++;
    vertices[num] = x2 + offset;
    num++;
    vertices[num] = y2;
    num++;
    vertices[num] = z2;
    num++;
    vertices[num] = R;
    num++;
    vertices[num] = G;
    num++;
    vertices[num] = B;
    num++;
    vertices[num] = flag ? (0 - x2) : 0;
    num++;
    vertices[num] = flag ? (0 - y2) : 1;
    num++;
    vertices[num] = 0;
    num++;
    vertices[num] = x3 + offset;
    num++;
    vertices[num] = y3;
    num++;
    vertices[num] = z3;
    num++;
    vertices[num] = R;
    num++;
    vertices[num] = G;
    num++;
    vertices[num] = B;
    num++;
    vertices[num] = flag ? (0 - x3) : 0;
    num++;
    vertices[num] = flag ? (0 - y3) : 1;
    num++;
    vertices[num] = 0;
    num++;
}

class ball
{
public:
    ball(float rr, float RR, float GG, float BB)
    {
        r = rr;
        R = RR;
        G = GG;
        B = BB;
        x = 0;
        y = 0;
        z = 0;
        triangle(x + r, y + 0.0f, z + 0.0f, x + 0.0f, y + r, z + 0.0f, x + 0.0f, y + 0.0f, z + r, 1);
        triangle(x - r, y + 0.0f, z + 0.0f, x + 0.0f, y + 0.0f, z + r, x + 0.0f, y + r, z + 0.0f, 1);
        triangle(x + r, y + 0.0f, z + 0.0f, x + 0.0f, y + 0.0f, z + r, x + 0.0f, y - r, z + 0.0f, 1);
        triangle(x + r, y + 0.0f, z + 0.0f, x + 0.0f, y + 0.0f, z - r, x + 0.0f, y + r, z + 0.0f, 1);
        triangle(x - r, y + 0.0f, z + 0.0f, x + 0.0f, y - r, z + 0.0f, x + 0.0f, y + 0.0f, z + r, 1);
        triangle(x + r, y + 0.0f, z + 0.0f, x + 0.0f, y + 0.0f, z - r, x + 0.0f, y - r, z + 0.0f, 1);
        triangle(x - r, y + 0.0f, z + 0.0f, x + 0.0f, y + r, z + 0.0f, x + 0.0f, y + 0.0f, z - r, 1);
        triangle(x - r, y + 0.0f, z + 0.0f, x + 0.0f, y + 0.0f, z - r, x + 0.0f, y - r, z + 0.0f, 1);
    }
    void triangle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, unsigned int depth)
    {
        float x4, y4, z4, x5, y5, z5, x6, y6, z6, t;
        if (depth < DEPTH)
        {
            x4 = (x1 + x2) / 2;
            y4 = (y1 + y2) / 2;
            z4 = (z1 + z2) / 2;
            t = sqrt((x4 - x) * (x4 - x) + (y4 - y) * (y4 - y) + (z4 - z) * (z4 - z));
            x4 = x + (x4 - x) / t * r;
            y4 = y + (y4 - y) / t * r;
            z4 = z + (z4 - z) / t * r;
            x5 = (x3 + x2) / 2;
            y5 = (y3 + y2) / 2;
            z5 = (z3 + z2) / 2;
            t = sqrt((x5 - x) * (x5 - x) + (y5 - y) * (y5 - y) + (z5 - z) * (z5 - z));
            x5 = x + (x5 - x) / t * r;
            y5 = y + (y5 - y) / t * r;
            z5 = z + (z5 - z) / t * r;
            x6 = (x1 + x3) / 2;
            y6 = (y1 + y3) / 2;
            z6 = (z1 + z3) / 2;
            t = sqrt((x6 - x) * (x6 - x) + (y6 - y) * (y6 - y) + (z6 - z) * (z6 - z));
            x6 = x + (x6 - x) / t * r;
            y6 = y + (y6 - y) / t * r;
            z6 = z + (z6 - z) / t * r;
            triangle(x1, y1, z1, x4, y4, z4, x6, y6, z6, depth + 1);
            triangle(x4, y4, z4, x5, y5, z5, x6, y6, z6, depth + 1);
            triangle(x4, y4, z4, x2, y2, z2, x5, y5, z5, depth + 1);
            triangle(x6, y6, z6, x5, y5, z5, x3, y3, z3, depth + 1);
        }
        else
        {
            vertices[num] = x1;
            num++;
            vertices[num] = y1;
            num++;
            vertices[num] = z1;
            num++;
            vertices[num] = R;
            num++;
            vertices[num] = G;
            num++;
            vertices[num] = B;
            num++;
            vertices[num] = x1;
            num++;
            vertices[num] = y1;
            num++;
            vertices[num] = z1;
            num++;
            vertices[num] = (x1 + r) / (r * 2);
            num++;
            vertices[num] = (y1 + r) / (r + 2);
            num++;
            vertices[num] = x2;
            num++;
            vertices[num] = y2;
            num++;
            vertices[num] = z2;
            num++;
            vertices[num] = R;
            num++;
            vertices[num] = G;
            num++;
            vertices[num] = B;
            num++;
            vertices[num] = x2;
            num++;
            vertices[num] = y2;
            num++;
            vertices[num] = z2;
            num++;
            vertices[num] = (x2 + r) / (r * 2);
            num++;
            vertices[num] = (y2 + r) / (r + 2);
            num++;
            vertices[num] = x3;
            num++;
            vertices[num] = y3;
            num++;
            vertices[num] = z3;
            num++;
            vertices[num] = R;
            num++;
            vertices[num] = G;
            num++;
            vertices[num] = B;
            num++;
            vertices[num] = x3;
            num++;
            vertices[num] = y3;
            num++;
            vertices[num] = z3;
            num++;
            vertices[num] = (x3 + r) / (r * 2);
            num++;
            vertices[num] = (y3 + r) / (r + 2);
            num++;
        }
    }
private:
    float r, x, y, z, R, G, B;
};

static ball BALL(5.0f, 0.0f, 0.0f, 0.0f);

class Shader
{
public:
    unsigned int ID;
    Shader(const char* verPath, const char* fragPath)
    {
        string vertexCode, fragmentCode;
        ifstream vShaderFile, fShaderFile;
        vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
        fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
        try
        {
            vShaderFile.open(verPath);
            fShaderFile.open(fragPath);
            stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vShaderFile.close();
            fShaderFile.close();
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            cout << "vertex着色器内容如下：\n" << vertexCode << endl;
            cout << endl;
            cout << "fragment着色器内容如下：\n" << fragmentCode << endl;
            cout << endl;
        }
        catch (ifstream::failure error)
        {
            cout << "错误：Shader读取文件失败" << endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        unsigned int vertex, fragment;
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "vertex");
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(vertex, "fragment");
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    void use()
    {
        glUseProgram(ID);
    }
    void SetInt(const string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void SetFloat(const string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    void SetMat(const string& name, const glm::mat4& value) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
    }
    void SetVec(const string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
private:
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                cout << "错误：Shader编译错误: " << type << "\n" << infoLog << "\n ------------------------------------------------------- " << endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                cout << "错误：Shader链接错误: " << type << "\n" << infoLog << "\n ------------------------------------------------------- " << endl;
            }
        }
    }
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)//回调函数，适应窗口大小调整
{
    glViewport(0, 0, width, height);
    WIDTH = width;
    HEIGHT = height;
    projection = perspective(80.0f, float(WIDTH) / float(HEIGHT), 0.1f, 100.0f);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "IMT", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    num++;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    Shader myshader("IMT.vert", "IMT.frag");
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        myshader.SetMat("Projection", projection);
        myshader.SetMat("View", view);
        myshader.SetMat("Model", model);
        myshader.SetVec("pointLight.pose", pointLight.pose);
        myshader.SetVec("pointLight.intensity", pointLight.intensity);
        myshader.SetFloat("pointLight.a0", pointLight.a0);
        myshader.SetFloat("pointLight.a1", pointLight.a1);
        myshader.SetFloat("pointLight.a2", pointLight.a2);
        myshader.SetVec("dirLight.direction", dirLight.direction);
        myshader.SetVec("dirLight.intensity", dirLight.intensity);
        myshader.SetVec("spotLight.pos", spotLight.pos);
        myshader.SetVec("spotLight.direction", spotLight.direction);
        myshader.SetVec("spotLight.intensity", spotLight.intensity);
        myshader.SetFloat("spotLight.alpha", spotLight.alpha);
        myshader.SetFloat("spotLight.cutoff", spotLight.cutoff);
        myshader.SetFloat("spotLight.a0", spotLight.a0);
        myshader.SetFloat("spotLight.a1", spotLight.a1);
        myshader.SetFloat("spotLight.a2", spotLight.a2);
        myshader.SetFloat("material.ka", material.ka);
        myshader.SetFloat("material.kd", material.kd);
        myshader.SetFloat("material.ks", material.ks);
        myshader.SetFloat("material.shiness", material.shiness);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, num * sizeof(float), vertices, GL_STATIC_DRAW);
        //定义顶点位置属性
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
        //定义顶点颜色属性
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
        //定义顶点法线属性
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
        //定义顶点纹理属性
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
        glDrawArrays(GL_TRIANGLES, 0, num / 11);
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load("./wood.jpg", &width, &height, &nrChannels, 0);
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        myshader.SetInt("textures", 0);
        stbi_image_free(data);
        glGenerateMipmap(GL_TEXTURE_2D);
        myshader.use();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}
