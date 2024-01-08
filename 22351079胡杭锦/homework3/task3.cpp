#include <glad/glad.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "camera.h"
#include "shader.h"
//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>
#include "sphere.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "model.h"
#include "mesh.h"
#include <math.h>
using namespace std;
using namespace glm;
using namespace Assimp;

//帧时间控制
float deltaTime = 0.0f;	// 两帧间隔
float lastFrame = 0.0f; // 上一帧时间

// 借助camera类实现相机控制
Camera camera= Camera(vec3(0.0f, 5.0f, 25.0f));
float lastX = 1280 / 2.0f;
float lastY = 720 / 2.0f;
bool firstMouse = true;
//相机模式切换控制符
bool controlTmp = true;
int Control = 0;
bool tmp = false;
vec3 lightpos = vec3(-1.0f,7.0f,1.0f);

int acc1=0,acc2=0,acc3=0,acc4=0,acc5=0;

//处理模型
mat4 processModel(mat4 model){

    if(Control==1){
        acc1+=1;
    }
    if(Control==2){
        acc1-=1;
    }
    if(Control==3){
        acc2+=1;
    }
    if(Control==4){
        acc2-=1;
    }

    if(Control==5){
        acc3+=1;}

    if(Control==8){
        acc3-=1;
    }

    if(Control==6){
        acc4+=1;}

    if(Control==9){
        acc4-=1;
    }

    if(Control==7){
        acc5+=1;}

    if(Control==10){
        acc5-=1;
    }

        //按照变换顺序
        lightpos = vec3(-1.0f+0.1f*(acc3/10),7.0f+0.1f*(acc4/10),1.0f+0.1f*(acc5/10));
        model = translate(model,vec3(0.1f*(acc3/10),0.1f*(acc4/10),0.1f*(acc5/10)));
        model = scale(model,vec3(1.0f+0.01f*(acc1/10)));
        model = rotate(model, radians(3.0f*(acc2/10)),vec3(0.0f,1.0f,0.0f));

        Control = 0;
    return model;
}

mat4 processModel2(mat4 model){


    //按照变换顺序
    model = rotate(model, radians(-3.0f*(acc2/10)),vec3(0.0f,1.0f,0.0f));
//   model = translate(model,vec3(-sqrt(577)* sin(-3.0f*(acc2/10)*PI/180),0,sqrt(577)* cos(-3.0f*(acc2/10)*PI/180)));
    model = translate(model,vec3(-0.1f*(acc3/10),-0.1f*(acc4/10),-0.1f*(acc5/10)));


    Control = 0;
    return model;
}




//控制相机移动和esc退出
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(up,deltaTime,controlTmp);
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(down,deltaTime,controlTmp);
    if(controlTmp){
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime,controlTmp);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime,controlTmp);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime,controlTmp);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime,controlTmp);
    }else{
        if(glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
            Control = 1;
        if(glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
            Control = 2;
        if(glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
            Control = 3;
        if(glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
            Control = 4;
        if(glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS){
            Control = 5;
//            camera.ProcessKeyboard(RIGHT, deltaTime,controlTmp);
        }
        if(glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS){
            Control = 6;
//            camera.ProcessKeyboard(up,deltaTime,controlTmp);
        }
        if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
            Control = 7;
//            camera.ProcessKeyboard(BACKWARD, deltaTime,controlTmp);
        }
        if(glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS){
            Control = 8;
//            camera.ProcessKeyboard(LEFT, deltaTime,controlTmp);
        }
        if(glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS){
            Control = 9;
//            camera.ProcessKeyboard(down, deltaTime,controlTmp);
        }
        if(glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS){
            Control = 10;
//            camera.ProcessKeyboard(FORWARD, deltaTime,controlTmp);
        }
    }
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS){
        controlTmp = !controlTmp;
        acc1=0,acc2=0,acc3=0,acc4=0,acc5=0;
        lightpos = vec3(-1.0f,7.0f,1.0f);
        camera = Camera(vec3(0.0f, 5.0f, 25.0f));
    }

}


// 鼠标回调，移动摄像头视角
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
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    if(controlTmp){
    camera.ProcessMouseMovement(xoffset, yoffset);
    }

}
// 鼠标滑轮回调，缩放控制
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

//将一些初始化进行二次封装
//纹理载入 利用stb_img库
unsigned int loadTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        //绑定纹理
        glBindTexture(GL_TEXTURE_2D, textureID);
        //生成纹理
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        //设置重复
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //设置mipmap和线性插值方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //释放图像内存
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
//assimp模型加载函数
//处理模型节点//废弃，包含在model类中处理
//void processNode(aiNode *node, const aiScene *scene)
//{
//    //处理节点所有的网格
//    for(unsigned int i = 0; i < node->mNumMeshes; i++)
//    {
//        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
//        meshes.push_back(processMesh(mesh, scene));
//    }
//    //子节点重复
//    for(unsigned int i = 0; i < node->mNumChildren; i++)
//    {
//        processNode(node->mChildren[i], scene);
//    }
//}
//
//void loadModel(string path)
//{
//    Assimp::Importer import;
//    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
//
//    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
//    {
//        cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
//        return;
//    }
//    directory = path.substr(0, path.find_last_of('/'));
//
//    processNode(scene->mRootNode, scene);
//}


GLFWwindow* init(){
    //初始化
    glfwInit();
    //设置版本为opengl3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //设置为核心模式
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //设置窗口
    GLFWwindow* window = glfwCreateWindow(1280, 720, "task3", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    //绑定一系列外部硬件事件
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //初始化glad
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glViewport(0, 0, 1280, 720);
    return window;
}


int main(){
    //初始化
    GLFWwindow* window= init();
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



    //编译shader，借助shader类编译
    Shader shaderOBJ("..\\shader\\homework3_2.vs","..\\shader\\homework3_2.fs");
    Shader shaderMoon("..\\shader\\homework3_1.vs","..\\shader\\homework3_1.fs");



    Model Tree("..\\model\\tree\\Tree.obj");
    Model Grass("..\\model\\grass\\10450_Rectangular_Grass_Patch_v1_iterations-2.obj");
    Model Tower("..\\model\\obj\\wooden watch tower2.obj");
    //月亮
    unsigned int moonTexture = loadTexture("..\\img\\2k_moon.jpg");
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, moonTexture);
    Sphere sphere = Sphere();
    unsigned int VAO,VBO,EBO,normalVBO, textureVBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    //取出vector对象的size和data来获得长度和数组
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sphere.vertices.size() * sizeof(float), sphere.vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.indices.size() * sizeof(int), sphere.indices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &normalVBO);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, sphere.normals.size() * sizeof(float), sphere.normals.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &textureVBO);
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
    glBufferData(GL_ARRAY_BUFFER, sphere.texcoords.size() * sizeof(float), sphere.texcoords.data(), GL_STATIC_DRAW);

    //链接属性
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    //解绑，防止对VAO的误操作
    glBindVertexArray(0);


    while(!glfwWindowShouldClose(window))
    {
        //获得每一帧的时间
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //接收按键操作
        processInput(window);
        //清除缓存
        glClearColor(0.0f, 0.0f, 0.0f,0.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        // 透视和投影矩阵
        mat4 projection = perspective(radians(camera.Zoom), (float)(1280.0f / 720.0f), 0.1f, 300.0f);
        mat4 view = camera.GetViewMatrix(controlTmp);
        mat4 model = mat4(1.0f);



        //生成塔楼
        model = mat4(1.0f);
        model = scale(model, vec3(1.0f, 1.0f, 1.0f));
        model = translate(model,vec3(-1.0f,-1.0f,1.0f));
        model = processModel(model);
        view = processModel2(view);
        shaderOBJ.use();

        shaderOBJ.setVec3("dirLight.direction", -1.0f, -2.0f, 5.0f);
        shaderOBJ.setVec3("dirLight.ambient", 0.01f, 0.01f, 0.01f);
        shaderOBJ.setVec3("dirLight.diffuse", 0.3f, 0.3f, 0.3f);

        shaderOBJ.setVec3("pointLight.position", lightpos.x,lightpos.y,lightpos.z);
        shaderOBJ.setVec3("pointLight.ambient", 1.0f*0.05f, 0.8f*0.05f, 0.2f*0.05f);
        shaderOBJ.setVec3("pointLight.diffuse", 1.0f, 0.8f, 0.2f);
        shaderOBJ.setFloat("pointLight.constant", 1.0f);
        shaderOBJ.setFloat("pointLight.linear", 0.045f);
        shaderOBJ.setFloat("pointLight.quadratic", 0.0075f);

        shaderOBJ.setInt("material.diffuse", 0);
        shaderOBJ.setFloat("material.shininess", 32.0f);

        shaderOBJ.setMat4("projection", projection);
        shaderOBJ.setMat4("view", view);
        shaderOBJ.setMat4("model", model);
        Tower.Draw(shaderOBJ);

        //生成月亮
        shaderMoon.use();
        shaderMoon.setInt("texture_diffuse1",10);
        shaderMoon.setMat4("view",view);
        shaderMoon.setMat4("projection", projection);
        model = mat4(1.0f);
//        model = translate(model,vec3(sunEarthDistance*cos((acc%36)*15.0f),0,sunEarthDistance*sin((acc%36)*15.0f)));
        model = scale(model,vec3(5.0f));
        model = translate(model,vec3(10.0f,20.0f,-50.0f));
        shaderMoon.setMat4("model", model);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sphere.indices.size(), GL_UNSIGNED_INT, 0);

        //生成草地
        model = mat4(1.0f);
        model = translate(model,vec3(0,-3.8f,0));
        model = scale(model, vec3(0.5f, 0.5f, 0.5f));
        model = rotate(model, radians(-90.0f),vec3(1.0f,0.0f,0.0f));
        shaderOBJ.use();

        shaderOBJ.setVec3("dirLight.direction", -1.0f, -2.0f, 5.0f);
        shaderOBJ.setVec3("dirLight.ambient", 0.01f, 0.01f, 0.01f);
        shaderOBJ.setVec3("dirLight.diffuse", 0.3f, 0.3f, 0.3f);

        shaderOBJ.setVec3("pointLight.position", lightpos.x,lightpos.y,lightpos.z);
        shaderOBJ.setVec3("pointLight.ambient", 1.0f*0.05f, 0.8f*0.05f, 0.2f*0.05f);
        shaderOBJ.setVec3("pointLight.diffuse", 1.0f, 0.8f, 0.2f);
        shaderOBJ.setFloat("pointLight.constant", 1.0f);
        shaderOBJ.setFloat("pointLight.linear", 0.045f);
        shaderOBJ.setFloat("pointLight.quadratic", 0.0075f);

        shaderOBJ.setInt("material.diffuse", 0);
        shaderOBJ.setFloat("material.shininess", 32.0f);

        shaderOBJ.setMat4("projection", projection);
        shaderOBJ.setMat4("view", view);
        shaderOBJ.setMat4("model", model);
        Grass.Draw(shaderOBJ);


      //随机生成100棵树 含有透明贴图后画,不然会产生混合问题
        srand(142);
        float randomX=0;
        float randomZ=0;
      for(int i = 0;i<100;i++){
          if(i%2==0){
              randomX = 2+50*rand()/(RAND_MAX+1.0);
          }
          else{
              randomX = -2-50*rand()/(RAND_MAX+1.0);
          }
          if(i<50)
              randomZ = -50*rand() /(RAND_MAX+1.0);
          else
              randomZ = 50*rand() /(RAND_MAX+1.0);
          mat4 model = mat4(1.0f);
          float rt = rand() /(RAND_MAX+1.0);
          model = scale(model, vec3(1.0f, 1.0f, 1.0f));
          model = translate(model,vec3(randomX,0.0f,randomZ));
          model = rotate(model,radians(360.0f*rt),vec3(0.0f,1.0f,0.0f));
          shaderOBJ.use();

          shaderOBJ.setVec3("dirLight.direction", -1.0f, -2.0f, 5.0f);
          shaderOBJ.setVec3("dirLight.ambient", 0.01f, 0.01f, 0.01f);
          shaderOBJ.setVec3("dirLight.diffuse", 0.3f, 0.3f, 0.3f);

          shaderOBJ.setVec3("pointLight.position", lightpos.x,lightpos.y,lightpos.z);
          shaderOBJ.setVec3("pointLight.ambient", 1.0f*0.05f, 0.8f*0.05f, 0.2f*0.05f);
          shaderOBJ.setVec3("pointLight.diffuse", 1.0f, 0.8f, 0.2f);
          shaderOBJ.setFloat("pointLight.constant", 1.0f);
          shaderOBJ.setFloat("pointLight.linear", 0.045f);
          shaderOBJ.setFloat("pointLight.quadratic", 0.0075f);

          shaderOBJ.setInt("material.diffuse", 0);
          shaderOBJ.setFloat("material.shininess", 32.0f);

          shaderOBJ.setMat4("projection", projection);
          shaderOBJ.setMat4("view", view);
          shaderOBJ.setMat4("model", model);
          Tree.Draw(shaderOBJ);
        }


        glfwPollEvents();
        glfwSwapBuffers(window);

    }


    glfwTerminate();
    return 0;
}
