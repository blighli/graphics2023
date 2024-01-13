/**
 * Shader to render object.
 */

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "file_process.h"
#include "user_exception.hpp"

/**
 * Shader to render object in OpenGL.
 * Examples:
 *   Shader shader(v_path, f_path);
 *   shader.use();
 *   shader.setMat4("mvp", mvp);
 */
class Shader {
 public:
  /**
   * Init shader.
   * @param vertex_shader_source_file_path file path of vertex shader
   * @param fragment_shader_source_file_path file path of fragment shader
   */
  Shader(const char *vertex_shader_source_file_path, const char *fragment_shader_source_file_path);

  /**
   * Deconstruct
   */
  ~Shader();

  /**
   * Use program.
   */
  void Use();

  /**
   * Uniform set bool.
   * @param name uniform param name
   * @param value uniform param value
   */
  void SetBool(const std::string &name, bool value) const;

  /**
   * Uniform set int.
   * @param name uniform param name
   * @param value uniform param value
   */
  void SetInt(const std::string &name, int value) const;

  /**
   * Uniform set float.
   * @param name uniform param name
   * @param value uniform param value
   */
  void SetFloat(const std::string &name, float value) const;

  /**
   * Uniform set mat3.
   * @param name uniform param name
   * @param value uniform param value
   */
  void SetMat3(const std::string &name, glm::mat3 &value) const;

  /**
   * Uniform set mat4.
   * @param name uniform param name
   * @param value uniform param value
   */
  void SetMat4(const std::string &name, glm::mat4 &value) const;

  /**
   * Uniform set vec3.
   * @param name uniform param name
   * @param value uniform param value
   */
  void SetVec3(const std::string &name, glm::vec3 &value) const;

  /**
   * Uniform set vec3.
   * @param name uniform param name
   * @param x uniform param value x
   * @param y uniform param value y
   * @param z uniform param value z
   */
  void SetVec3(const std::string &name, float x, float y, float z) const;

  /**
   * Get shader program id.
   * @return shader program id
   */
  [[nodiscard]] unsigned int GetProgramId() const;

 private:
  /**
   * Utility function for checking shader compilation/linking errors.
   * @param shader shader id
   * @param type shader type(vertex, fragment or program)
   */
  void CheckShaderErrors(unsigned int shader, std::string type);

  // program ID
  unsigned int program_id_;
};


#endif //SHADER_H
