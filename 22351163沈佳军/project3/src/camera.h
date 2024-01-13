// Created by lonelyinnovator on 2023/2/21 15:53.

#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <algorithm>
#include <filesystem>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <opencv2/opencv.hpp>


// defines several possible options for camera movement
enum class CameraMovement {
  kForward,
  kBackward,
  kLeft,
  kRight,
  kUp,
  kDown
};

/*default camera values*/

// camera default yaw angle
const float kYaw = -90.0f;
// camera default pitch angle
const float kPitch = 0.0f;
// camera default roll angle
const float kRoll = 0.0f;
// camera default speed angle
const float kSpeed = 2.5f;
// camera default sensitivity angle
const float kSensitivity = 0.1f;
// camera default zoom
const float kZoom = 60.0f;


class Camera {
 public:
  /**
   * Default Camera constructor.
   */
  Camera();

  /**
   * Camera constructor, position required.
   * @param position camera position
   * @param world_up world up vector
   * @param yaw camera yaw angle
   * @param pitch camera pitch angle
   * @param roll camera roll angle
   */
  explicit Camera(const glm::vec3 &position, const glm::vec3 &world_up = glm::vec3(0.0f, 1.0f, 0.0f),
                  float yaw = kYaw, float pitch = kPitch, float roll = kRoll);

  /**
   * Camera constructor with float.
   * @param pos_x position X
   * @param pos_y position Y
   * @param pos_z position Z
   * @param up_x world up X
   * @param up_y world up Y
   * @param up_z world up z
   * @param yaw yaw angle
   * @param pitch pitch angle
   * @param roll roll angle
   */
  Camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw = kYaw,
         float pitch = kPitch,
         float roll = kRoll);

  /**
   * Deconstruct
   */
  ~Camera();

  /**
   * Get camera view matrix (lookAt matrix).
   * @return camera view matrix
   */
  [[nodiscard]] glm::mat4 GetViewMatrix() const;

  /**
   * Use keyboard to move camera.
   * @param direction camera move direction
   * @param deltaTime move deltaTime
   */
  void ProcessKeyboard(CameraMovement direction, float deltaTime);

  /**
   * Use mouse to rotate camera.
   * @param offset_x mouse offsetX(left, right)
   * @param offset_y mouse offsetY(front, back)
   * @param constrain_pitch if constrain pitch from -90 to 90
   */
  void ProcessMouseMovement(float offset_x, float offset_y, GLboolean constrain_pitch = true);

  /**
   * Use mouse wheel to zoom camera.
   * @param offset_y mouse up wheel
   */
  void ProcessMouseScroll(float offset_y);

  /**
   * Print camera info.
   */
  void PrintCameraInfo();

  /**
   * Snapshot opengl window to image with width and height, and write image with filename to output_dir.
   * @param filename image filename
   * @param output_dir output directory of image
   * @param width image width
   * @param height image height
   * @param is_rgb true: RGB image; false: Gray image
   */
  static void Snapshot(const std::string &filename, const std::string &output_dir, int width, int height, bool is_rgb);

  /**
   * Get camera front vector.
   * @return camera front vector
   */
  glm::vec3 &GetFront();

  /**
   * Set camera front vector and update camera angles by vector.
   * @param front camera front vector
   */
  void SetFront(const glm::vec3 &front);

  /**
   * Get camera up vector.
   * @return camera up vector
   */
  glm::vec3 &GetUp();

  /**
   * Get camera right vector.
   * @return camera right vector
   */
  glm::vec3 &GetRight();

  /**
   * Get camera yaw angle.
   * @return camera yaw angle
   */
  [[nodiscard]] float GetYaw() const;

  /**
   * Set camera yaw angle.
   * @param yaw camera yaw angle
   */
  void SetYaw(float yaw);

  /**
   * Get pitch angle.
   * @return camera pitch angle
   */
  [[nodiscard]] float GetPitch() const;

  /**
   * Set camera pitch angle.
   * @param pitch camera pitch angle
   */
  void SetPitch(float pitch);

  /**
   * Get camera roll angle.
   * @return camera roll angle
   */
  [[nodiscard]] float GetRoll() const;

  /**
   * Set camera roll.
   * @param roll camera roll angle
   */
  void SetRoll(float roll);

  /**
   * Get world up vector.
   * @return world up vector
   */
  glm::vec3 &GetWorldUp();

  /**
   * Set world up vector.
   * @param world_up world up vector
   */
  void SetWorldUp(const glm::vec3 &world_up);

  /**
   * Get camera position.
   * @return
   */
  glm::vec3 &GetPosition();

  /**
   * Set camera position.
   * @param position camera position
   */
  void SetPosition(const glm::vec3 &position);

  /**
   * Get camera move speed.
   * @return camera move speed
   */
  [[nodiscard]] float GetMovementSpeed() const;

  /**
   * Set camera move speed.
   * @param movement_speed camera move speed
   */
  void SetMovementSpeed(float movement_speed);

  /**
   * Get mouse sensitivity(mouse move factor).
   * @return mouse sensitivity
   */
  [[nodiscard]] float GetMouseSensitivity() const;

  /**
   * Set mouse sensitivity.
   * @param mouse_sensitivity mouse sensitivity
   */
  void SetMouseSensitivity(float mouse_sensitivity);

  /**
   * Get camera zoom.
   * @return camera zoom
   */
  [[nodiscard]] float GetZoom() const;

  /**
   * Set camera zoom.
   * @param zoom camera zoom
   */
  void SetZoom(float zoom);


 private:
  /**
   * Update camera vectors.
   */
  void UpdateCameraVectors();

  /**
   * Update camera vectors(front, up, right) by angles(yaw, pitch, roll)
   */
  void UpdateCameraVectorsByAngles();

  /**
   * Update camera vectors(yaw, pitch, roll) by angles(front, up, right)
   */
  void UpdateCameraAnglesByVectors();

  // camera position
  glm::vec3 position_{glm::vec3(0.0f, 0.0f, 3.0f)};
  // camera move speed(for keyboard)
  float movement_speed_{kSpeed};
  // camera rotation sensitivity(for mouse)
  float mouse_sensitivity_{kSensitivity};
  // camera zoom factor(for mouse)
  float zoom_{kZoom};

  /*camera vectors*/

  // camera world up vector
  glm::vec3 world_up_{glm::vec3(0.0f, 1.0f, 0.0f)};
  // camera front vector
  glm::vec3 front_{glm::vec3(0.0f, 0.0f, -1.0f)};
  glm::vec3 up_{};
  glm::vec3 right_{};

  /*euler angles*/

  // camera yaw
  float yaw_{kYaw};
  float pitch_{kPitch};
  float roll_{kRoll};

};


#endif //CAMERA_H
