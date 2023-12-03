#pragma once
#include <string>

class Shader
{
private:
    // 程序ID
    unsigned int ID;
    //检查着色器程序是否编译、链接成功
    void checkCompileErrors(unsigned int ID, std::string type);
public:
    // 读取、编译shader文件的函数
    Shader(const char* vertexPath, const char* fragmentPath);
    // 使用shader文件的函数
    void use();
    // uniform工具函数
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
};
