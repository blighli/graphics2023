#include "Planet.h"
#include "stb_image.h"

Planet::Planet() {
    planetShader = new Shader("shader.vts", "shader.fgs");
    starJudge = false;
}

void Planet::initVertex() {
    for (int y = 0; y <= Y_SEGMENTS; y++)
    {
        for (int x = 0; x <= X_SEGMENTS; x++)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = R * std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = R * std::cos(ySegment * PI);
            float zPos = R * std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float xTex = 1.0 * x / X_SEGMENTS;
            float yTex = 1 - 1.0 * y / Y_SEGMENTS;
            sphereVertices.push_back(xPos);
            sphereVertices.push_back(yPos);
            sphereVertices.push_back(zPos);
            sphereVertices.push_back(xPos);
            sphereVertices.push_back(yPos);
            sphereVertices.push_back(zPos);
            sphereVertices.push_back(xTex);
            sphereVertices.push_back(yTex);
        }
    }
    
    for (int i = 0; i < Y_SEGMENTS; i++)
    {
        for (int j = 0; j < X_SEGMENTS; j++)
        {
            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);

            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j + 1);
        }
    }

}

void Planet::init(char* path, float R) {
    this->R = R;
    initVertex();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);

    
    glGenBuffers(1, &element_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(int), &sphereIndices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    diffuseMap = loadTexture(path);
    specularMap = loadTexture(path);

}

void Planet::setStarLighting(glm::vec3 viewPos, float ambient1, float ambient2, float ambient3,
    float diffuse1, float diffuse2, float diffuse3,
    float specular1, float specular2, float specular3) {
    planetShader->use();
    planetShader->setInt("material.diffuse", 0);
    planetShader->setInt("material.specular", 1);

    planetShader->setVec3("viewPos", viewPos);
    planetShader->setVec3("light.position", xStar, yStar, zStar);
    if (starJudge) {
        planetShader->setVec3("light.ambient", diffuse1, diffuse2, diffuse3);
    }
    else {
        planetShader->setVec3("light.ambient", ambient1, ambient2, ambient3);
    }
    planetShader->setVec3("light.diffuse", diffuse1, diffuse2, diffuse3);
    planetShader->setVec3("light.specular", specular1, specular2, specular3);

    planetShader->setInt("material.diffuse", 0);
    planetShader->setInt("material.specular", 1);
    planetShader->setInt("material.shininess", 64.0f);
}

void Planet::setModel(float T,
    float rRevolution,
    float xStar, float yStar, float zStar,
    float xRotation, float yRotation, float zRotation,
    float xSpin, float ySpin, float zSpin) { 

    float angleRevolution, angleSpin;
    angleRevolution = 360.0 * T / TRevolution;
    angleSpin = 360.0 * T / TSpin;

    this->xStar = xStar;
    this->yStar = yStar;
    this->zStar = zStar;

    model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(xStar, yStar, zStar));
    if (!starJudge) {
        model = glm::rotate(model, glm::radians(angleRevolution),
            glm::normalize(glm::vec3(xRotation, yRotation, zRotation)));
        model = glm::translate(model, glm::vec3(rRevolution, 0.0, 0.0));
    }
    model = glm::rotate(model, glm::radians(angleSpin),
        glm::normalize(glm::vec3(xSpin, ySpin, zSpin)));
    
}
void Planet::setStarJudge(bool starJudge) {
    this->starJudge = starJudge;
}


void Planet::setTRevolutionTSpin(float TRevolution, float TSpin) {
    this->TRevolution = TRevolution;
    this->TSpin = TSpin;
}

void Planet::draw(glm::mat4 projection, glm::mat4 view) {
    planetShader->setMat4("projection", projection);
    planetShader->setMat4("view", view);

    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);

    glBindVertexArray(VAO);

    planetShader->setMat4("model", model);

    glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
}

unsigned int Planet::loadTexture(char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        cout << "path: " << path << endl;
        std::cout << "Failed to load texture" << std::endl;
    }

    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   

    return textureID;


}
