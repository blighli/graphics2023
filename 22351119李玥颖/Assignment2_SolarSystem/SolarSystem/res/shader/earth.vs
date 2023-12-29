#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 u_MVP;
uniform mat4 model;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aPos;
    TexCoords = aTexCoord;
    gl_Position = u_MVP * vec4(aPos, 1.0);
}
