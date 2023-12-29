#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include <algorithm>

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;
const float NEAR_DISTANCE = 0.1f;
const float FAR_DISTANCE = 1000.0f;

#define MAX_BONE_INFLUENCE 4
#define PI (3.14159265358979323846f)

struct Vertex {
    
    glm::vec3 Position;
    
    glm::vec3 Normal;
    
    glm::vec2 TexCoords;
    
    glm::vec3 Tangent;
    
    glm::vec3 Bitangent;
    
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};
