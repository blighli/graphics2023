#version 330 core

layout (location = 0) in vec3 aPos;
layout(location=1) in vec3 acolor;

out vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 up;
uniform vec3 cross;
uniform vec3 center;

void main()
{
    color = acolor;
    vec3 v1 = vec3(cross.x * aPos.x, cross.y * aPos.x, cross.z * aPos.x);
    vec3 v2 = vec3(up.x * aPos.y, up.y * aPos.y, up.z * aPos.y);
    vec3 pos = v1 + v2;
    pos += center;
    gl_Position = projection * view * model * vec4(pos, 1.0);
}