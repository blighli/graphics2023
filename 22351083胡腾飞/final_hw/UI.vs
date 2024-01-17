#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = projection * vec4(aPos.x, aPos.y, 0.0f, 1.0f);
}
