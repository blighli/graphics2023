#pragma once
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define GL_GLEXT_PROTOTYPES
#include <bits/stdc++.h>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <GLFW/glfw3.h>

using namespace std;

GLuint createTexture(const string& filename)
{
    int components;
    GLuint texID;
    int tWidth, tHeight;

    //stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename.c_str(), &tWidth, &tHeight, &components, 0);
    puts(filename.c_str());
    assert(data != NULL && "load texture error");
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    if (components == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tWidth, tHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    else if (components == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tWidth, tHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Clean up
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    return texID;
}

//Use program before loading texture
//	texUnit can be - GL_TEXTURE0, GL_TEXTURE1, etc...
void loadTexture(GLuint texID, GLuint texUnit, GLuint program, const string& uniformName)
{
    glActiveTexture(texUnit);
    glBindTexture(GL_TEXTURE_2D, texID);

    GLuint uniformLocation = glGetUniformLocation(program, uniformName.c_str());
    glUniform1i(uniformLocation, 0);

    assert(glGetError() == GL_NO_ERROR && "loadTexture error");
}
