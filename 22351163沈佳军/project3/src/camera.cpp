// Created by lonelyinnovator on 2023/2/21 15:53.

#include "camera.h"

Camera::Camera() {
  UpdateCameraVectorsByAngles();
}

Camera::Camera(const glm::vec3 &position, const glm::vec3 &world_up, float yaw, float pitch, float roll) {
  position_ = position;
  world_up_ = world_up;
  yaw_ = yaw;
  pitch_ = pitch;
  UpdateCameraVectorsByAngles();
  SetRoll(roll);
}

Camera::Camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch,
               float roll) {
  position_ = glm::vec3(pos_x, pos_y, pos_z);
  world_up_ = glm::vec3(up_x, up_y, up_z);
  yaw_ = yaw;
  pitch_ = pitch;
  UpdateCameraVectorsByAngles();
  SetRoll(roll);
}


Camera::~Camera() = default;


glm::mat4 Camera::GetViewMatrix() const {
  // use camera position, target position, word up vector to create LookAt matrix
  return glm::lookAt(position_, position_ + front_, up_);
}

void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime) {
  float velocity = movement_speed_ * deltaTime;
  if (direction == CameraMovement::kForward)
    position_ += front_ * velocity;
  if (direction == CameraMovement::kBackward)
    position_ -= front_ * velocity;
  if (direction == CameraMovement::kLeft)
    position_ -= right_ * velocity;
  if (direction == CameraMovement::kRight)
    position_ += right_ * velocity;
  if (direction == CameraMovement::kUp)
    position_ += up_ * velocity;
  if (direction == CameraMovement::kDown)
    position_ -= up_ * velocity;
}

void Camera::ProcessMouseMovement(float offset_x, float offset_y, GLboolean constrain_pitch) {
  offset_x *= mouse_sensitivity_;
  offset_y *= mouse_sensitivity_;

  yaw_ += offset_x;
  pitch_ += offset_y;

  if (constrain_pitch) {
    pitch_ = std::clamp(pitch_, -89.0f, 89.0f);
  }

  UpdateCameraVectorsByAngles();
}

void Camera::ProcessMouseScroll(float offset_y) {
  zoom_ -= offset_y;
  zoom_ = std::clamp(zoom_, 1.0f, 45.0f);
}

void Camera::PrintCameraInfo() {
  std::cout << "Position: " << position_.x << " " << position_.y << " " << position_.z << "; ";
  std::cout << "Pitch: " << GetPitch() << "; Yaw: " << GetYaw() << "; ";
  std::cout << "Front: " << GetFront().x << " " << GetFront().y << " " << GetFront().z << std::endl;
}

void Camera::Snapshot(const std::string &filename, const std::string &output_dir, int width, int height, bool is_rgb) {
  // create output dir if not exists
  if (!std::filesystem::exists(output_dir)) {
    std::filesystem::create_directories(output_dir);
  }
  // create opencv image
  cv::Mat img(height, width, CV_8UC3);
  glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);
  // opencv mat: BGR
  glReadPixels(0, 0, img.cols, img.rows, GL_BGR, GL_UNSIGNED_BYTE, img.data);
  // flip image
  cv::Mat flipped;
  // flip top-bottom
  cv::flip(img, flipped, 0);
  // not rgb image, use gray image
  if (!is_rgb) {
    cv::cvtColor(flipped, flipped, cv::COLOR_RGB2GRAY);

  }
  cv::imwrite(output_dir + "\\" + filename, flipped);
  std::cout << output_dir + "\\" + filename << " saved" << std::endl;
}

glm::vec3 &Camera::GetUp() {
  return up_;
}

glm::vec3 &Camera::GetRight() {
  return right_;
}

glm::vec3 &Camera::GetFront() {
  return front_;
}

void Camera::SetFront(const glm::vec3 &front) {
  front_ = front;
  UpdateCameraAnglesByVectors();
}

glm::vec3 &Camera::GetWorldUp() {
  return world_up_;
}

void Camera::SetWorldUp(const glm::vec3 &world_up) {
  world_up_ = world_up;
  UpdateCameraVectors();
}

float Camera::GetYaw() const {
  return yaw_;
}

void Camera::SetYaw(float yaw) {
  yaw_ = yaw;
  UpdateCameraVectorsByAngles();
}

float Camera::GetPitch() const {
  return pitch_;
}

void Camera::SetPitch(float pitch) {
  pitch_ = pitch;
  UpdateCameraVectorsByAngles();
}

float Camera::GetRoll() const {
  return roll_;
}

void Camera::SetRoll(float roll) {
  roll_ = roll;
  glm::mat4 rot(1.0f);
  up_ = glm::vec3(glm::rotate(rot, glm::radians(roll_), front_) * glm::vec4(up_, 1.0f));
  SetWorldUp(up_);
}

glm::vec3 &Camera::GetPosition() {
  return position_;
}

void Camera::SetPosition(const glm::vec3 &position) {
  position_ = position;
}

float Camera::GetMovementSpeed() const {
  return movement_speed_;
}

void Camera::SetMovementSpeed(float movement_speed) {
  movement_speed_ = movement_speed;
}

float Camera::GetMouseSensitivity() const {
  return mouse_sensitivity_;
}

void Camera::SetMouseSensitivity(float mouse_sensitivity) {
  mouse_sensitivity_ = mouse_sensitivity;
}

float Camera::GetZoom() const {
  return zoom_;
}

void Camera::SetZoom(float zoom) {
  zoom_ = zoom;
}

void Camera::UpdateCameraVectors() {
  right_ = glm::normalize(glm::cross(front_, world_up_));
  up_ = glm::normalize(glm::cross(right_, front_));
}

void Camera::UpdateCameraVectorsByAngles() {
  glm::vec3 front;
  front.x = cos(glm::radians(pitch_)) * cos(glm::radians(yaw_));
  front.y = sin(glm::radians(pitch_));
  front.z = cos(glm::radians(pitch_)) * sin(glm::radians(yaw_));

  front_ = glm::normalize(front);
  UpdateCameraVectors();
}

void Camera::UpdateCameraAnglesByVectors() {
  UpdateCameraVectors();

  float p = asin(front_.y);
  float y = asin(front_.z / cos(p));

  pitch_ = p * 180 / glm::pi<float>();
  yaw_ = y * 180 / glm::pi<float>();

  std::cout << pitch_ << std::endl;
  std::cout << yaw_ << std::endl;
}
