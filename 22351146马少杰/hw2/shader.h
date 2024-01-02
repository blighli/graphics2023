#pragma once
#include <bits/stdc++.h>
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
using namespace std;

GLuint CompileShader(const GLenum shaderType, const string& filename)
{
    string shader_code;

    ifstream input(filename.c_str());
    assert(input && "Shader file not exist");
    copy(istreambuf_iterator<char>(input),
        istreambuf_iterator<char>(),
        back_inserter(shader_code));
    input.close();

    GLuint shaderObject = glCreateShader(shaderType);
    const GLchar* source_ptr = shader_code.c_str();
    glShaderSource(shaderObject, 1, &source_ptr, 0);
    glCompileShader(shaderObject);

    GLint status;
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
    assert(status && "Compile shader error");

    return shaderObject;
}

GLuint createShader()
{
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, "vertex.glsl");
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, "fragment.glsl");

    GLuint programObject = glCreateProgram();
    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);
    glLinkProgram(programObject);

    GLint status;
    glGetProgramiv(programObject, GL_LINK_STATUS, &status);
    assert(status && "Link shader error");

    return programObject;
}