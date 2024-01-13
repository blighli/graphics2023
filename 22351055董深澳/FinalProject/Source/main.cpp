#include <iostream>
#include <fstream>
#include <string>

#include "glad/glad.h"  //
#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#else
#include <SDL.h>
#include <SDL_opengl.h>
#endif
#include <cstdio>

#define GLM_FORCE_RADIANS
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"

#include "ParticleSystem.h"
#include "FileLoader.h"
#include "ExportTools.h"
#include "UserControl.h"

using namespace std;

int screen_width, screen_height;
float aspect;
bool fullscreen;
bool saveOutput;

// 着色器
const GLchar* vertexSource;
const GLchar* fragmentSource;

// 相机参数
glm::vec3 cam_loc, look_at, up;

// 球体建模
float rocket_rad, ptc_rad, tail_rad;
int sph_vert, ico_vert; // 顶点数

GLint uniModel, uniView, uniProj, uniColor;

void init();
void update(float dt);
void computePhysics(float dt);
void set_camera();
void draw_sphere();
void draw_ico();

class firework {
public:
    glm::vec3 position;
    int stage; // 状态，0为发射，1为爆炸
    float delay; // 第一次发射前的延迟

    vector<glm::vec3> sphere_loc; // 粒子球体
    vector<glm::vec3> sphere_vel; // 球体速度
    vector<glm::vec3> sphere_col; // 球体颜色

    vector<float> sphere_life; // 球体生存时间
    vector<float> sphere_life_ini; // 球体初始生存时间

    ParticleSystem tail;

    firework() { 
        position = glm::vec3(0.0f, 0.0f, 0.0f);
        interval = 4.0f;
        delay = 3 * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        timer = interval;
        stage = 0;
        sphere_loc.push_back(position);
        sphere_vel.push_back(glm::vec3(0.0f + noise(1.0f), 0.0f + noise(1.0f), 15.0f + noise(0.5f)));
        sphere_col.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

        expl_count = 200;
        r_life = 1.6f;
        p_life = 1.5f;
        color_type = int(3 * static_cast <float> (rand()) / static_cast <float> (RAND_MAX));

        sphere_life.push_back(r_life);
        sphere_life_ini.push_back(r_life);

        tail = ParticleSystem(2500, 0.3f, 0.1f, 10000, position, rocket_rad / 4.0f, src_type::dim3, axis::Z, -1.0f, 20.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        tail.set_collision(false);
    }

    firework(glm::vec3 pos, float t, float rlife, float plife, int explc) { 
        position = pos;
        interval = t;
        delay = 3 * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        timer = interval;
        stage = 0;
        sphere_loc.push_back(position);
        sphere_vel.push_back(glm::vec3(0.0f + noise(0.1f), 0.0f + noise(0.1f), 15.0f + noise(0.1f)));
        sphere_col.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

        expl_count = explc;
        r_life = rlife;
        p_life = plife;
        color_type = int(3 * static_cast <float> (rand()) / static_cast <float> (RAND_MAX));

        sphere_life.push_back(rlife);
        sphere_life_ini.push_back(r_life);

        tail = ParticleSystem(2500, 0.3f, 0.1f, 10000, position, rocket_rad / 4.0f, src_type::dim3, axis::Z, -1.0f, 20.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        tail.set_collision(false);
    }

    void update(float dt) {
        if (delay > 0) delay -= dt;
        else {
            timer -= dt;
            for (int i = 0; i < sphere_loc.size(); i++) {
                sphere_loc[i] += (sphere_vel[i] * dt);
                sphere_vel[i].z += (g * dt);
                sphere_life[i] -= dt;
            }
            // 发射阶段
            if (stage == 0) {
                tail.set_src_pos(sphere_loc[0]); 
                if (sphere_life[0] <= 0) {
                    explode();
                }
            }
            // 爆炸阶段
            else {
                float damping_fac = 0.95f;
                vector<int> del_list;
                for (int i = sphere_loc.size() - 1; i >= 0; i--) {
                    sphere_vel[i] *= damping_fac; 
                    if (sphere_life[i] <= 0) del_list.push_back(i);
                }
                for (int i : del_list) { 
                    del_sphere(i);
                }
            }
            tail.update(dt, ParticleSystem::particle_type::others, glm::vec3(0.0f, 0.0f, 0.0f), 0.0f);

            // 重新启动
            if (timer <= 0) {
                timer = interval;
                stage = 0;
                g = -9.8f;
                sphere_loc.push_back(position);
                sphere_vel.push_back(glm::vec3(0.0f + noise(0.1f), 0.0f + noise(0.1f), 15.0f + noise(0.1f)));
                sphere_col.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
                color_type = int(3 * static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
                sphere_life.push_back(1.6f);
                sphere_life_ini.push_back(1.6f);

                tail.set_gen(true);
                tail.set_src_pos(sphere_loc[0]);
            }
        }

    }

private:
    float g = -9.8f;

    float interval; // 两次发射的间隔
    float timer;

    int expl_count; // 爆炸阶段的粒子数
    float r_life; // 烟花生存时间
    float p_life; // 粒子生存时间

    int color_type;


    // 爆炸阶段
    void explode() {
        stage = 1; // 调整阶段
        g = -7.0f;
        tail.set_gen(false); 
        float explosion_speed = 9.0f;
        for (int i = 0; i < expl_count; i++) {
            sphere_loc.push_back(sphere_loc[0]);
            sphere_vel.push_back(explosion_speed * sample_vel());
            float life = p_life + noise(0.4f);
            sphere_life_ini.push_back(life);
            sphere_life.push_back(life);
        }
        generate_color();
        // 删除第一个球体
        del_sphere(0);
    }

    // 删除球体
    void del_sphere(int i) {
        int last = sphere_loc.size() - 1;
        sphere_loc[i] = sphere_loc[last];
        sphere_loc.pop_back();
        sphere_vel[i] = sphere_vel[last];
        sphere_vel.pop_back();
        sphere_col[i] = sphere_col[last];
        sphere_col.pop_back();
        sphere_life[i] = sphere_life[last];
        sphere_life.pop_back();
        sphere_life_ini[i] = sphere_life_ini[last];
        sphere_life_ini.pop_back();
    }


    float noise(float scale) {
        return scale * static_cast <float> (rand()) / static_cast <float> (RAND_MAX) - 0.5;
    }

    // 设置球体速度
    glm::vec3 sample_vel() {
        float theta = 2 * M_PI * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);  // 0 - 2PI
        float phi = M_PI * static_cast <float> (rand()) / static_cast <float> (RAND_MAX); // 0 - PI
        float noise = 1 - (0.1f * static_cast <float> (rand()) / static_cast <float> (RAND_MAX)); // 0.9 - 1
        return noise * glm::vec3(cos(theta) * sin(phi), sin(theta) * sin(phi), cos(phi));
    }

    // 设置颜色
    void generate_color() {
        glm::vec3 col0;
        glm::vec3 col1;
        switch (color_type) {
        case 0:
            col0 = glm::vec3(1.0f, 0.0f, 0.0f); // red
            col1 = glm::vec3(0.0f, 0.0f, 1.0f); // blue
            break;
        case 1:
            col0 = glm::vec3(0.0f, 1.0f, 1.0f); // cyan
            col1 = glm::vec3(1.0f, 1.0f, 0.0f); // yellow
            break;
        default:
            col0 = glm::vec3(1.0f, 0.0f, 1.0f); // magenta
            col1 = glm::vec3(0.0f, 1.0f, 0.0f); // green
        }
        for (int i = 0; i < expl_count; i++) {
            if (noise(1.0f) > 0) sphere_col.push_back(col0);
            else sphere_col.push_back(col1);
        }
    }
};

const int fw_num = 5;
firework fw[fw_num];

int main(int argc, char* argv[]) {

    init();

    // 初始化
    SDL_Init(SDL_INIT_VIDEO); 

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    // 创建窗口
    SDL_Window* window = SDL_CreateWindow("Firework", 100, 100, screen_width, screen_height, SDL_WINDOW_OPENGL);
    aspect = screen_width / (float)screen_height; 


    SDL_GLContext context = SDL_GL_CreateContext(window);

    if (gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        printf("\nOpenGL loaded\n");
        printf("Vendor:   %s\n", glGetString(GL_VENDOR));
        printf("Renderer: %s\n", glGetString(GL_RENDERER));
        printf("Version:  %s\n\n", glGetString(GL_VERSION));
    }
    else {
        printf("ERROR: Failed to initialize OpenGL context.\n");
        return -1;
    }


    // 加载着色器
    init();

    // 顶点着色器
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    loadShader(vertexShader, "Shader/ptc_vertexshader.txt");

    // 片段着色器
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    loadShader(fragmentShader, "Shader/ptc_fragmentshader.txt");



    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor"); 
    glLinkProgram(shaderProgram);

    glUseProgram(shaderProgram);

    // 加载模型
    std::vector< float > vertices;
    std::vector< float > uvs; 
    std::vector< float > normals;
    loadobj("Assets/sphere.obj", vertices, uvs, normals);
    sph_vert = vertices.size();
    loadobj("Assets/icosphere.obj", vertices, uvs, normals); 
    ico_vert = vertices.size();



    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo[2];
    glGenBuffers(2, vbo);  

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    glEnableVertexAttribArray(posAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_STATIC_DRAW);

    GLint normalAttrib = glGetAttribLocation(shaderProgram, "inNormal");
    glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(normalAttrib);


    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    uniModel = glGetUniformLocation(shaderProgram, "model");
    uniView = glGetUniformLocation(shaderProgram, "view");
    uniProj = glGetUniformLocation(shaderProgram, "proj");

    // 事件循环
    SDL_Event windowEvent;
    bool quit = false;
    float lastTime = SDL_GetTicks() / 1000.f;
    float dt = 0;
    while (!quit) {
        while (SDL_PollEvent(&windowEvent)) {
            if (windowEvent.type == SDL_QUIT) quit = true; 
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE)
                quit = true; //Exit event loop
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_f) 
                fullscreen = !fullscreen;
            SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);

            move_camera(windowEvent, cam_loc, look_at, up, 0.1f, 0.08f);
        }
        bound_rotate(window, cam_loc, look_at, 0.003f);

        glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!saveOutput) dt = (SDL_GetTicks() / 1000.f) - lastTime;
        if (dt > .1) dt = .1; 
        lastTime = SDL_GetTicks() / 1000.f;
        if (saveOutput) dt += .07; 

        update(dt);

        if (saveOutput) Win2PPM(screen_width, screen_height);


        SDL_GL_SwapWindow(window); 
    }


    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    glDeleteBuffers(1, vbo);

    glDeleteVertexArrays(1, &vao);

    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return 0;
}

void init() {
    screen_width = 800;
    screen_height = 600;
    saveOutput = false;
    fullscreen = false;

    cam_loc = glm::vec3(15.f, 15.f, 1.8f);
    look_at = glm::vec3(0.0f, 0.0f, 5.0f);
    up = glm::vec3(0.0f, 0.0f, 1.0f);

    rocket_rad = 0.13f;
    ptc_rad = 0.1f;
    tail_rad = 0.1f;

    fw[0] = firework();
    fw[1] = firework(glm::vec3(-5.0f, 0.0f, 0.0f), 5.0f, 1.6f, 1.5f, 200);
    fw[2] = firework(glm::vec3(0.0f, -5.0f, 0.0f), 7.0f, 1.6f, 1.5f, 200);
    fw[3] = firework(glm::vec3(-5.0f, 5.0f, 0.0f), 6.0f, 1.6f, 1.5f, 200);
    fw[4] = firework(glm::vec3(5.0f, -5.0f, 0.0f), 5.0f, 1.6f, 1.5f, 200);
};

void update(float dt) {

    set_camera();

    draw_sphere();
    draw_ico();

    computePhysics(dt);
}

void computePhysics(float dt) {
    for (int f = 0; f < fw_num; f++) {
        fw[f].update(dt);
    }
}

void set_camera() {
    // 设置相机参数
    glm::mat4 proj = glm::perspective(3.14f / 4, aspect, .1f, 100.0f); //FOV, aspect, near, far
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

    glm::mat4 view = glm::lookAt(cam_loc, look_at, up); 
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

    glPointSize(8.0f);
}

void draw_sphere() {
    for (int f = 0; f < fw_num; f++) {
        if (fw[f].delay > 0) continue;
        for (int i = 0; i < fw[f].sphere_loc.size(); i++) {
            glm::mat4 model = glm::mat4();
            model = glm::translate(model, fw[f].sphere_loc[i]);
            if (fw[f].stage == 0) {
                model = glm::scale(model, glm::vec3(rocket_rad));
                glUniform3f(uniColor, fw[f].sphere_col[i].r, fw[f].sphere_col[i].g, fw[f].sphere_col[i].b);
            }
            else {
                model = glm::scale(model, glm::vec3(ptc_rad));
                float mult = 1 - (fw[f].sphere_life[i] / fw[f].sphere_life_ini[i]);
                glUniform3f(uniColor, mult + (1 - mult) * fw[f].sphere_col[i].r, mult + (1 - mult) * fw[f].sphere_col[i].g, mult + (1 - mult) * fw[f].sphere_col[i].b);
            }
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, sph_vert / 3);
        }
    }
}

void draw_ico() {
    for (int f = 0; f < fw_num; f++) {
        for (int i = 0; i < fw[f].tail.Pos.size(); i++) {
            glm::mat4 model = glm::mat4();
            model = glm::translate(model, fw[f].tail.Pos[i]);
            model = glm::scale(model, glm::vec3(tail_rad));
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3f(uniColor, fw[f].tail.Clr[i].r, fw[f].tail.Clr[i].g, fw[f].tail.Clr[i].b);
            glDrawArrays(GL_TRIANGLES, sph_vert / 3, ico_vert / 3); //(Primitives, starting index, Number of vertices)
        }
    }
}