#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 in_uv;

out vec4 out_color;
out vec2 uv;

void main()
{
    gl_Position = vec4(pos, 1.0);
    out_color = color;
    uv = in_uv;
    //out_color = vec4(1.0, 0.0, 0.0, 1.0);
    //out_color = color;
}