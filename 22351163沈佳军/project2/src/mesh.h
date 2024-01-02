// Created by lonelyinnovator on 2023/2/21 23:56.
/**
 * Mesh.
 */

#ifndef MESH_H
#define MESH_H

#include <algorithm>
#include <string>
#include <vector>
#include <map>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shader.h"
#include "general_algorithm.h"

#define MAX_BONE_INFLUENCE 4

/**
 * Mesh vertex.
 */
struct Vertex {
  // position
  glm::vec3 position;
  // normal
  glm::vec3 normal;
  // texture coords
  glm::vec2 tex_coords;
  // tangent
  glm::vec3 tangent;
  // bi_tangent
  glm::vec3 bi_tangent;
  // bone indexes which will influence this vertex
  int m_bone_ids[MAX_BONE_INFLUENCE];
  // weights from each bone
  float m_weights[MAX_BONE_INFLUENCE];
};

/**
 * Mesh texture.
 */
struct Texture {
  unsigned int id;
  std::string type;
  std::string path;
};

/**
 * Mesh, load mesh and draw mesh in OpenGL
 * To Load mesh, see in Model.h
 * Examples:
 *   Mesh mesh(vertices, indices, texture, seg);
 *   mesh.Draw(shader);
 */
class Mesh {
 public:
  /**
   * Mesh constructor.
   * @param vertices mesh vertices
   * @param indices mesh indices
   * @param textures mesh textures
   * @param triangle_seg mesh triangle segmentation
   */
  Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::vector<Texture> &textures);

  /**
   * Deconstruct
   */
  ~Mesh();

  /**
   * Render the mesh on OpenGL.
   * @param shader mesh shader
   */
  void Draw(Shader &shader);

  /**
   * Delete all object(vao, vbo, ebo).
   */
  void DeleteObject();

  /**
   * Get mesh vertices.
   * @return mesh vertices
   */
  [[nodiscard]] const std::vector<Vertex> &GetVertices() const;

  /**
   * Get mesh indices.
   * @return mesh indices
   */
  [[nodiscard]] const std::vector<unsigned int> &GetIndices() const;

  /**
   * Get mesh textures.
   * @return mesh textures
   */
  [[nodiscard]] const std::vector<Texture> &GetTextures() const;

  /**
   * Get VAO.
   * @return VAO
   */
  [[nodiscard]] unsigned int GetVao() const;

  /**
   * Get VBO.
   * @return VBO
   */
  [[nodiscard]] unsigned int GetVbo() const;

  /**
   * Get EBO.
   * @return EBO
   */
  [[nodiscard]] unsigned int GetEbo() const;

 private:
  /**
   * initializes all the buffer objects/arrays
   */
  void SetupMesh();

  // mesh vertices
  std::vector<Vertex> vertices_;
  // triangle indices, [a, b, c, d, ...]
  std::vector<unsigned int> indices_;
  // mesh textures
  std::vector<Texture> textures_;

  // render data
  unsigned int vao_{0}, vbo_{0}, ebo_{0};
};


#endif //MESH_H
