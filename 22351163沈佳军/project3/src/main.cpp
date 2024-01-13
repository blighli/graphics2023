#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION

#include "shader.h"
#include "model.h"
#include "camera.h"
#include "general_algorithm.h"

void init();

void framebuffer_size_callback(GLFWwindow *wd, int width, int height);

void process_input(GLFWwindow *wd, Model &m);

void on_mouse_move(GLFWwindow *wd, double pos_x_in, double pos_y_in);

void on_mouse_scroll(GLFWwindow *wd, double offset_x, double offset_y);

const unsigned int kScreenWidth = 800;
const unsigned int kScreenHeight = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), kYaw, kPitch);

float delta_time = 0.0f, last_frame = 0.0f;

float last_x = kScreenWidth / 2.0f, last_y = kScreenHeight / 2.0f;
bool first_mouse = true;

GLFWwindow *window;


int main() {

  // init opengl and glfw
  init();

  // load airplane model
  Model model(R"(obj/airplane.obj)");

  // choose shader: point_light shader or parallel_light shader
  Shader obj_shader = Shader("shaders/point_light.vert", "shaders/point_light.frag");

  glm::vec3 light_position(0.0, 0.0f, 30.0f);

  // render loop
  while (!glfwWindowShouldClose(window)) {
    auto current_frame = static_cast<float>(glfwGetTime());
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    process_input(window, model);

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
    obj_shader.SetVec3("light.position", light_position);
    obj_shader.SetVec3("light.ambient", 0.3f, 0.3f, 0.3f);
    obj_shader.SetVec3("light.diffuse", 0.6f, 0.6f, 0.6f);
    obj_shader.SetVec3("light.specular", 1.0f, 1.0f, 1.0f);
    obj_shader.SetFloat("light.constant", 1.0f);
    obj_shader.SetFloat("light.linear", 0.014f);
    obj_shader.SetFloat("light.quadratic", 0.0007f);
    // set view position
    obj_shader.SetVec3("viewPos", camera.GetPosition());

    for (auto mesh: model.GetMeshes()) {
      // set mvp matrix
      glm::mat4 model_matrix = mesh.GetModelMatrix();
      obj_shader.SetMat4("model", model_matrix);
      glm::mat3 normal_matrix = glm::transpose(glm::inverse(model_matrix));
      obj_shader.SetMat3("normalMatrix", normal_matrix);
      glm::mat4 view = camera.GetViewMatrix();
      obj_shader.SetMat4("view", view);
      glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), kScreenWidth * 1.0f / kScreenHeight,
                                              0.1f, 100.0f);
      obj_shader.SetMat4("projection", projection);

      mesh.Draw(obj_shader);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // save last window snapshot
//  GeneralAlgorithm::Snapshot("snapshot.png", R"(../assets)", kScreenWidth, kScreenHeight, true);

  glDeleteProgram(obj_shader.GetProgramId());
//  glDeleteProgram(light_shader.GetProgramId());

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
  window = glfwCreateWindow(kScreenWidth, kScreenHeight, "ObjViewer", nullptr, nullptr);
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

void framebuffer_size_callback(GLFWwindow *wd, int width, int height) {
  glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *wd, Model &m) {
  // esc: exit
  if (glfwGetKey(wd, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(wd, true);
  // w: camera move forward
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(CameraMovement::kForward, delta_time);
  // s: camera move backward
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(CameraMovement::kBackward, delta_time);
  // a: camera move left
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(CameraMovement::kLeft, delta_time);
  // d: camera move right
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(CameraMovement::kRight, delta_time);
  // e: camera move upward
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    camera.ProcessKeyboard(CameraMovement::kUp, delta_time);
  // q: camera move downward
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    camera.ProcessKeyboard(CameraMovement::kDown, delta_time);

  // u: model move forward along X axis
  if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
    for (auto &mesh: m.GetMeshes())
      mesh.ProcessKeyboard(MeshTransformation::kXForward, delta_time);
  // j: model move backward along X axis
  if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
    for (auto &mesh: m.GetMeshes())
      mesh.ProcessKeyboard(MeshTransformation::kXBackward, delta_time);
  // i: model move forward along Y axis
  if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
    for (auto &mesh: m.GetMeshes())
      mesh.ProcessKeyboard(MeshTransformation::kYForward, delta_time);
  // k: model move backward along Y axis
  if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
    for (auto &mesh: m.GetMeshes())
      mesh.ProcessKeyboard(MeshTransformation::kYBackward, delta_time);
  // o: model move forward along Z axis
  if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    for (auto &mesh: m.GetMeshes())
      mesh.ProcessKeyboard(MeshTransformation::kZForward, delta_time);
  // l: model move backward along Z axis
  if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    for (auto &mesh: m.GetMeshes())
      mesh.ProcessKeyboard(MeshTransformation::kZBackward, delta_time);

  // r: model rotate forward along X axis
  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    for (auto &mesh: m.GetMeshes())
      mesh.ProcessKeyboard(MeshTransformation::kXForRotate, delta_time);
  // f: model rotate backward along X axis
  if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    for (auto &mesh: m.GetMeshes())
      mesh.ProcessKeyboard(MeshTransformation::kXBackRotate, delta_time);
  // t: model rotate forward along Y axis
  if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    for (auto &mesh: m.GetMeshes())
      mesh.ProcessKeyboard(MeshTransformation::kYForRotate, delta_time);
  // g: model rotate forward along Y axis
  if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    for (auto &mesh: m.GetMeshes())
      mesh.ProcessKeyboard(MeshTransformation::kYBackRotate, delta_time);
  // y: model rotate forward along Z axis
  if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    for (auto &mesh: m.GetMeshes())
      mesh.ProcessKeyboard(MeshTransformation::kZForRotate, delta_time);
  // h: model rotate backward along Z axis
  if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    for (auto &mesh: m.GetMeshes())
      mesh.ProcessKeyboard(MeshTransformation::kZBackRotate, delta_time);

  // n: model zoom in
  if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
    for (auto &mesh: m.GetMeshes())
      mesh.ProcessKeyboard(MeshTransformation::kZoomIn, delta_time);
  // m: model zoom out
  if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    for (auto &mesh: m.GetMeshes())
      mesh.ProcessKeyboard(MeshTransformation::kZoomOut, delta_time);

}

void on_mouse_move(GLFWwindow *wd, double pos_x_in, double pos_y_in) {
  // camera rotation
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