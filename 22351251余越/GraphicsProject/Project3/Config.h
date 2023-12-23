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
    // 位置
    glm::vec3 Position;
    // 法线
    glm::vec3 Normal;
    // 纹理
    glm::vec2 TexCoords;
    // 切线
    glm::vec3 Tangent;
    // 副切线
    glm::vec3 Bitangent;
    // 骨骼索引
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    // 骨骼权重
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};
