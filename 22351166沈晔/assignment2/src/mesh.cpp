// Mesh：使用Learn OpenGL网站提供的头文件
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../header/shader.h"
#include "../header/mesh.h"
#include <string>
#include <utility>
#include <vector>
using namespace std;

Vertex::Vertex(glm::vec3 pos, glm::vec3 nor, glm::vec2 tex)
    : Position(pos), Normal(nor), TexCoords(tex), Tangent(0), Bitangent(0) {}

// constructor
Mesh::Mesh() = default;
Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures) {
  this->vertices = std::move(vertices);
  this->indices = std::move(indices);
  this->textures = std::move(textures);

  setupMesh();
}

// render the mesh
void Mesh::Draw(Shader &shader) {
  unsigned int diffuseNr = 1;
  unsigned int specularNr = 1;
  unsigned int normalNr = 1;
  unsigned int heightNr = 1;
  for (unsigned int i = 0; i < textures.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i); 
    string number;
    string name = textures[i].type;
    if (name == "texture_diffuse")
      number = std::to_string(diffuseNr++);
    else if (name == "texture_specular")
      number = std::to_string(specularNr++); // transfer unsigned int to stream
    else if (name == "texture_normal")
      number = std::to_string(normalNr++); // transfer unsigned int to stream
    else if (name == "texture_height")
      number = std::to_string(heightNr++); // transfer unsigned int to stream

    // now set the sampler to the correct texture unit
    glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
    // and finally bind the texture
    glBindTexture(GL_TEXTURE_2D, textures[i].id);
  }

  // draw mesh
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  // always good practice to set everything back to defaults
  // once configured.
  glActiveTexture(GL_TEXTURE0);
}

// initializes all the buffer objects/arrays
void Mesh::setupMesh() {
  // create buffers/arrays
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  // load data into vertex buffers
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // A great thing about structs is that their memory layout
  // is sequential for all its items. The effect is that we
  // can simply pass a pointer to the struct and it
  // translates perfectly to a glm::vec3/2 array which again
  // translates to 3/2 floats which translates to a byte
  // array.
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

  // set the vertex attribute pointers
  // vertex Positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  // vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));
  // vertex texture coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));
  // vertex tangent
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Tangent));
  // vertex bitangent
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Bitangent));

  glBindVertexArray(0);
}

#define M_PI 3.14159265358979323846

// 生成球的顶点和索引
void generate_sphere_vertices(
    vector<Vertex> &polygon_vertices, vector<unsigned int> &polygon_indices, int xymax = 100) {
  polygon_indices.clear();
  polygon_vertices.clear();

  const int maximum = (xymax + 1) * (xymax + 1);

  // 顶点数组：根据经纬线在球上取样
  for (int x = 0; x <= xymax; x++)
    for (int y = 0; y <= xymax; y++) {
      float xseg = (float)x / (float)xymax;
      float yseg = (float)y / (float)xymax;
      float xPos = std::cos(xseg * 2.0f * M_PI) * std::sin(yseg * M_PI);
      float yPos = std::cos(yseg * M_PI);
      float zPos = std::sin(xseg * 2.0f * M_PI) * std::sin(yseg * M_PI);
      glm::vec3 pos = glm::vec3(xPos, yPos, zPos);
      polygon_vertices.push_back(Vertex(pos, glm::normalize(pos), glm::vec2(xseg, yseg)));
    }

  // 索引数组：将顶点组成三角面片
  for (int i = 0; i <= xymax; i++)
    for (int j = 0; j <= xymax; j++) {
      polygon_indices.push_back((i * xymax + j) % maximum);
      polygon_indices.push_back(((i + 1) * xymax + j) % maximum);
      polygon_indices.push_back(((i + 1) * xymax + j + 1) % maximum);
      polygon_indices.push_back((i * xymax + j) % maximum);
      polygon_indices.push_back(((i + 1) * xymax + j + 1) % maximum);
      polygon_indices.push_back((i * xymax + j + 1) % maximum);
    }
}

Sphere::Sphere(int num_x, vector<Texture> textures) {
  generate_sphere_vertices(this->vertices, this->indices, num_x);
  this->textures = std::move(textures);
  this->setupMesh();
}