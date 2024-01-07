#include "mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    setupMesh();
}

void Mesh::Draw(Shader& shader)
{
    //绑定纹理
    uint32_t diffuseNr = 1;
    uint32_t specularNr = 1;
    uint32_t normalNr = 1;
    uint32_t heightNr = 1;
    for (uint32_t i = 0; i < textures.size(); ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string number;
        std::string name = textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++);
        else if (name == "texture_normal")
            number = std::to_string(normalNr++);
        else if (name == "texture_height")
            number = std::to_string(heightNr++);

        //设置对应采样
        glUniform1i(glGetUniformLocation(shader.getId(), (name + number).c_str()), i);
        //绑定纹理
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    //绘制mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<uint32_t>(indices.size()), GL_UNSIGNED_INT, 0);

    //设定完后重置为初始状态
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // 顶点位置
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // 顶点法线
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // 顶点纹理坐标
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

    glBindVertexArray(0);
}
