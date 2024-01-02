#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
// layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 FragNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {   
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0)); // 在世界坐标系中指定片元位置
    // TexCoords = aTexCoords;
    mat3 normalMatrix = mat3(transpose(inverse(model)));
    FragNormal = normalMatrix * aNormal;
}
