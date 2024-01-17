#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> 

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    // ����ID
    unsigned int ID;

    // ��������ȡ��������ɫ��
    Shader(const char* vertexPath, const char* fragmentPath);
    // ʹ��/�������
    void use();
    // uniform���ߺ���
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, float value1, float value2, float value3) const;
    void setVec3(const std::string& name, glm::vec3 vec) const;
    void setVec4(const std::string& name, float value1, float value2, float value3, float value4) const;
    void setM4fv(const std::string& name, glm::mat4 trans) const;
private:
    void checkCompileErrors(unsigned int shader, std::string type);
};

#endif