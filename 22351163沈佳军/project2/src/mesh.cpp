// Created by lonelyinnovator on 2023/2/21 23:56.

#include "mesh.h"

Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::vector<Texture> &textures) {
  this->vertices_ = vertices;
  this->indices_ = indices;
  this->textures_ = textures;

  // now that we have all the required data, set the vertex buffers and its attribute pointers.
  SetupMesh();
}

Mesh::~Mesh() = default;

void Mesh::Draw(Shader &shader) {
  // bind appropriate textures
  unsigned int diffuse_nr = 1;
  unsigned int specular_nr = 1;
  unsigned int normal_nr = 1;
  unsigned int height_nr = 1;
  for (unsigned int i = 0; i < textures_.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
    // retrieve texture number (the N in diffuse_textureN)
    std::string number;
    std::string name = textures_[i].type;
    if (name == "texture_diffuse")
      number = std::to_string(diffuse_nr++);
    else if (name == "texture_specular")
      number = std::to_string(specular_nr++); // transfer unsigned int to string
    else if (name == "texture_normal")
      number = std::to_string(normal_nr++); // transfer unsigned int to string
    else if (name == "texture_height")
      number = std::to_string(height_nr++); // transfer unsigned int to string

    // now set the sampler to the correct texture unit
    glUniform1i(glGetUniformLocation(shader.GetProgramId(), (name + number).c_str()), i);
    // and finally bind the texture
    glBindTexture(GL_TEXTURE_2D, textures_[i].id);
  }

  // draw mesh
  glBindVertexArray(vao_);
  glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices_.size()), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  // always good practice to set everything back to defaults once configured.
  glActiveTexture(GL_TEXTURE0);
}

void Mesh::DeleteObject() {
  glDeleteBuffers(1, &vbo_);
  glDeleteBuffers(1, &ebo_);
  glDeleteVertexArrays(1, &vao_);
}

const std::vector<Vertex> &Mesh::GetVertices() const {
  return vertices_;
}

const std::vector<unsigned int> &Mesh::GetIndices() const {
  return indices_;
}

const std::vector<Texture> &Mesh::GetTextures() const {
  return textures_;
}

unsigned int Mesh::GetVao() const {
  return vao_;
}

unsigned int Mesh::GetVbo() const {
  return vbo_;
}

unsigned int Mesh::GetEbo() const {
  return ebo_;
}

void Mesh::SetupMesh() {
  // create buffers/arrays
  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glGenBuffers(1, &ebo_);

  glBindVertexArray(vao_);
  // load data into vertex buffers
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  // A great thing about structs is that their memory layout is sequential for all its items.
  // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
  // again translates to 3/2 floats which translates to a byte array.
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);

  // set the vertex attribute pointers
  // vertex Positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
  // vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
  // vertex texture coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tex_coords));
  // vertex tangent
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tangent));
  // vertex bi_tangent
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, bi_tangent));
  // ids
  glEnableVertexAttribArray(5);
  glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void *) offsetof(Vertex, m_bone_ids));
  // weights
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, m_weights));

  glBindVertexArray(0);
}


