#include "shader.h"

Shader::Shader(const char *vertexShaderSourceFilePath, const char *fragmentShaderSourceFilePath) {
    // get shader source code from the file path
    std::string vertexShaderString;
    std::string fragmentShaderString;

    // source code file stream
    std::ifstream vertexShaderFile;
    std::ifstream fragmentShaderFile;

    // ensure ifstream objects can throw exceptions:
    vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        // open files
        vertexShaderFile.open(vertexShaderSourceFilePath);
        fragmentShaderFile.open(fragmentShaderSourceFilePath);
        // string stream
        std::stringstream vertexShaderStream, fragmentShaderStream;
        // read file's buffer contents into streams
        vertexShaderStream << vertexShaderFile.rdbuf();
        fragmentShaderStream << fragmentShaderFile.rdbuf();
        // close files
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // convert stream to string
        vertexShaderString = vertexShaderStream.str();
        fragmentShaderString = fragmentShaderStream.str();
    }
    catch (const std::ifstream::failure &e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }

    const char *vertexShaderSource = vertexShaderString.c_str();
    const char *fragmentShaderSource = fragmentShaderString.c_str();

    // shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    checkShaderErrors(vertexShader, "VERTEX");
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    checkShaderErrors(fragmentShader, "FRAGMENT");

    // shader program
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    checkShaderErrors(ID, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

}

void Shader::use() {
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int) value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}


void Shader::checkShaderErrors(unsigned int shader, std::string type) {
    int success;
    char info[1024];
    if (type == "PROGRAM") {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, info);
            std::cout << "Shader program linking failed of type " << type << std::endl << info << std::endl;
        }
    } else {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, info);
            std::cout << "Shader compile failed of type " << type << std::endl << info << std::endl;
        }
    }
}