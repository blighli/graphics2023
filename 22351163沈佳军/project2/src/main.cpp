#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION

#include "shader.h"
#include "model.h"
#include "camera.h"
#include "general_algorithm.h"

void init();

void gen_sphere(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, glm::vec3 origin, float radius);

glm::vec3 sphere_circle(glm::vec3 normal, glm::vec3 origin, float radius, float theta);

void framebuffer_size_callback(GLFWwindow *wd, int width, int height);

void process_input(GLFWwindow *wd);

void on_mouse_move(GLFWwindow *wd, double pos_x_in, double pos_y_in);

void on_mouse_scroll(GLFWwindow *wd, double offset_x, double offset_y);

const unsigned int kScreenWidth = 1000;
const unsigned int kScreenHeight = 1000;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), kYaw, kPitch);

float delta_time = 0.0f, last_frame = 0.0f;

float last_x = kScreenWidth / 2.0f, last_y = kScreenHeight / 2.0f;
bool first_mouse = true;

GLFWwindow *window;

int main() {

  init();

  std::vector<Vertex> earth_vertices, sun_vertices, moon_vertices;
  std::vector<unsigned int> earth_indices, sun_indices, moon_indices;
  std::vector<Texture> earth_textures, sun_textures, moon_textures;

  // generate earth sphere
  glm::vec3 earth_origin{0.0f, 0.0f, -5.0f};
  gen_sphere(earth_vertices, earth_indices, earth_origin, 1.0f);

  // generate sun sphere
  glm::vec3 sun_origin{0.0f, 0.0f, -15.0f};
  gen_sphere(sun_vertices, sun_indices, sun_origin, 3.0f);

  // generate moon sphere
  glm::vec3 moon_origin{0.0f, 0.0f, -3.0f};
  gen_sphere(moon_vertices, moon_indices, moon_origin, 0.2f);

  // load textures of earth, sun and moon
  unsigned int earth_texture_id, sun_texture_id, moon_texture_id;
  earth_texture_id = TextureFromFile("earth.png", "textures");
  sun_texture_id = TextureFromFile("sun.png", "textures");
  moon_texture_id = TextureFromFile("moon.png", "textures");
  earth_textures.push_back({earth_texture_id, "texture_diffuse", "textures/earth.png"});
  sun_textures.push_back({sun_texture_id, "texture_diffuse", "textures/sun.png"});
  moon_textures.push_back({moon_texture_id, "texture_diffuse", "textures/moon.png"});

  // generate mesh of earth, sun and moon
  Mesh earth_mesh(earth_vertices, earth_indices, earth_textures);
  Mesh sun_mesh(sun_vertices, sun_indices, sun_textures);
  Mesh moon_mesh(moon_vertices, moon_indices, moon_textures);

  // init obj shader and light shader
  Shader obj_shader = Shader("shaders/obj.vert", "shaders/obj.frag");
  Shader light_shader = Shader("shaders/light.vert", "shaders/light.frag");

  // render loop
  while (!glfwWindowShouldClose(window)) {
    auto current_frame = static_cast<float>(glfwGetTime());
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    process_input(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use obj shader
    obj_shader.Use();
    // set material
    glm::vec3 material_color(1.0f);
    obj_shader.SetVec3("material.ambient", material_color);
    obj_shader.SetVec3("material.diffuse", material_color);
    obj_shader.SetVec3("material.specular", material_color);
    obj_shader.SetFloat("material.shininess", 32.0f);
    // set point light
    obj_shader.SetVec3("pointLight.position", sun_origin);
    obj_shader.SetVec3("pointLight.ambient", 0.3f, 0.3f, 0.3f);
    obj_shader.SetVec3("pointLight.diffuse", 0.6f, 0.6f, 0.6f);
    obj_shader.SetVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
    obj_shader.SetFloat("pointLight.constant", 1.0f);
    obj_shader.SetFloat("pointLight.linear", 0.014f);
    obj_shader.SetFloat("pointLight.quadratic", 0.0007f);
    // set view position
    obj_shader.SetVec3("viewPos", camera.GetPosition());
    // earth revolution
    glm::mat4 model(1.0f);
    float theta = last_frame * 100.0f;
    glm::vec3 earth_rot = sphere_circle({0, 1, 0}, sun_origin, glm::distance(sun_origin, earth_origin),
                                        glm::radians(theta));
    model = glm::translate(model, earth_rot - earth_origin);
    obj_shader.SetMat4("model", model);
    glm::mat3 normal_matrix = glm::transpose(glm::inverse(model));
    obj_shader.SetMat3("normalMatrix", normal_matrix);
    glm::mat4 view = camera.GetViewMatrix();
    obj_shader.SetMat4("view", view);
    glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), kScreenWidth * 1.0f / kScreenHeight,
                                            0.1f, 100.0f);
    obj_shader.SetMat4("projection", projection);
    // draw earth
    earth_mesh.Draw(obj_shader);

    // moon revolution
    theta = last_frame * 150.0f;
    model = glm::mat4(1.0f);
    glm::vec3 moon_rot = sphere_circle({-1, 1, 1}, earth_rot, glm::distance(earth_origin, moon_origin),
                                       glm::radians(theta));
    model = glm::translate(model, moon_rot - moon_origin);
    obj_shader.SetMat4("model", model);
    normal_matrix = glm::transpose(glm::inverse(model));
    obj_shader.SetMat3("normalMatrix", normal_matrix);
    // draw moon
    moon_mesh.Draw(obj_shader);

    // use light shader to draw sun
    light_shader.Use();
    glm::mat4 light_model(1.0f);
    light_shader.SetMat4("model", light_model);
    glm::mat3 light_normal_matrix = glm::transpose(glm::inverse(light_model));
    light_shader.SetMat3("normalMatrix", light_normal_matrix);
    light_shader.SetMat4("view", view);
    light_shader.SetMat4("projection", projection);
    // draw light mesh
    sun_mesh.Draw(light_shader);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // save last window snapshot
//  GeneralAlgorithm::Snapshot("snapshot.png", R"(../assets)", kScreenWidth, kScreenHeight, true);

  glDeleteProgram(obj_shader.GetProgramId());
  glDeleteProgram(light_shader.GetProgramId());

  glfwTerminate();
  return 0;
}

void init() {
  // init glfw
  // ------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // msaa x4
  glfwWindowHint(GLFW_SAMPLES, 4);

  // create glfw window
  // -----------------------------
  window = glfwCreateWindow(kScreenWidth, kScreenHeight, "Earth", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create window" << std::endl;
    glfwTerminate();
    exit(-1);
  }
  glfwMakeContextCurrent(window);

  // load glad
  // ------------------------
  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
    std::cout << "Failed to init glad" << std::endl;
    exit(-1);
  }

  // set opengl viewport
  // ------------------------
  glViewport(0, 0, kScreenWidth, kScreenHeight);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glfwSetCursorPosCallback(window, on_mouse_move);
  glfwSetScrollCallback(window, on_mouse_scroll);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
}

void gen_sphere(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, glm::vec3 origin, float radius) {
  // generate sphere mesh based on origin and radius, return vertices and indices of sphere mesh
  vertices.clear();
  indices.clear();
  // alpha segments and theta segments
  int ma = 50, mt = 50;
  for (int i = 0; i <= ma; ++i) {
    float fi = static_cast<float>(i) / static_cast<float>(ma);
    for (int j = 0; j <= mt; ++j) {
      float fj = static_cast<float>(j) / static_cast<float>(mt);
      float alpha_rad = glm::radians(fi * 180.0f);
      float y = origin.y + radius * cos(alpha_rad);
      float theta_rad = glm::radians(fj * 360.0f);
      float z = origin.z + radius * sin(alpha_rad) * cos(theta_rad);
      float x = origin.x + radius * sin(alpha_rad) * sin(theta_rad);
      // position of point on sphere
      glm::vec3 v(x, y, z);
      // normal vector
      glm::vec3 n = v - origin;
      // texture coordinates
      glm::vec2 t(fj, fi);
      // add vertex
      vertices.push_back({v, n, t});
      // triangle indices of sphere
      if (i < ma && j < mt) {
        // first triangle
        indices.push_back(i * (mt + 1) + j);
        indices.push_back(i * (mt + 1) + j + 1);
        indices.push_back((i + 1) * (mt + 1) + j);
        // second triangle
        indices.push_back(i * (mt + 1) + j + 1);
        indices.push_back((i + 1) * (mt + 1) + j);
        indices.push_back((i + 1) * (mt + 1) + j + 1);
      }
    }
  }
}

glm::vec3 sphere_circle(glm::vec3 normal, glm::vec3 origin, float radius, float theta) {
  // center circle of sphere, tan(alpha) = -(normal_x * sin(theta) + normal_z * cos(theta)) / normal_y
  float alpha = atan(-(normal.x * sin(theta) + normal.z * cos(theta)) / normal.y);
  float x = origin.x + radius * cos(alpha) * sin(theta);
  float y = origin.y + radius * sin(alpha);
  float z = origin.z + radius * cos(alpha) * cos(theta);
  return {x, y, z};
}

void framebuffer_size_callback(GLFWwindow *wd, int width, int height) {
  glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *wd) {
  if (glfwGetKey(wd, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(wd, true);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(CameraMovement::kForward, delta_time);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(CameraMovement::kBackward, delta_time);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(CameraMovement::kLeft, delta_time);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(CameraMovement::kRight, delta_time);
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    camera.ProcessKeyboard(CameraMovement::kUp, delta_time);
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    camera.ProcessKeyboard(CameraMovement::kDown, delta_time);
}

void on_mouse_move(GLFWwindow *wd, double pos_x_in, double pos_y_in) {
  auto pos_x = static_cast<float>(pos_x_in);
  auto pos_y = static_cast<float>(pos_y_in);

  if (first_mouse) {
    last_x = pos_x;
    last_y = pos_y;
    first_mouse = false;
  }

  float offset_x = pos_x - last_x;
  float offset_y = last_y - pos_y;

  last_x = pos_x;
  last_y = pos_y;

  camera.ProcessMouseMovement(offset_x, offset_y);
}

void on_mouse_scroll(GLFWwindow *wd, double offset_x, double offset_y) {
  // offset_y: mouse scroll vertically
  camera.ProcessMouseScroll(static_cast<float>(offset_y));
}