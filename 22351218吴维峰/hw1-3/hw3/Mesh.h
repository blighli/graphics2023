#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Config.h"

using namespace std;

class Mesh {
public:
    
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);

    
    void Draw(Shader& shader);

private:
    
    unsigned int VBO, EBO;

    
    void setupMesh();
};
#endif