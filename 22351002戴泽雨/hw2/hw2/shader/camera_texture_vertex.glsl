#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 in_uv;
layout (location = 3) in vec3 in_normal;

out vec4 out_color;
out vec2 uv;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0);
    out_color = color;
    uv = in_uv;
    normal = in_normal;
    //out_color = vec4(1.0, 0.0, 0.0, 1.0);
    //out_color = color;
}