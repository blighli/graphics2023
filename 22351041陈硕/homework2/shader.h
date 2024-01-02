#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader
{
private:
    // 程序ID
    uint32_t ID;
    //检查着色器程序是否编译、链接成功
    void checkCompileErrors(uint32_t ID, std::string type);
public:
    // 读取、编译shader文件的函数
    Shader(const char* vertexPath, const char* fragmentPath);
    // 使用shader文件的函数
    void use();
    // uniform工具函数
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setVec4(const std::string& name, float x, float y, float z, float w) const;
    void setMat2(const std::string& name, const glm::mat2& mat) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    // id
    uint32_t getId() { return ID; }
};
