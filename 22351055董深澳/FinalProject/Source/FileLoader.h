#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "glad/glad.h"
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"

using namespace std;

string readFile(const char* filePath);

void loadShader(GLuint shaderID, const char* filepath);

void loadobj(const char* path, std::vector < float >& out_vertices,
    std::vector < float >& out_uvs,
    std::vector < float >& out_normals);

