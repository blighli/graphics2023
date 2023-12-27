#version 410 core

layout (vertices = 25) out;

uniform float outer_x, outer_y, inner_ver, inner_hori;

void main() {
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    // 设置曲面细分等级
    // 外部
    gl_TessLevelOuter[0] = outer_x; // 左
    gl_TessLevelOuter[1] = outer_y; // 下
    gl_TessLevelOuter[2] = outer_x; // 右
    gl_TessLevelOuter[3] = outer_y; // 上
    // 内部
    gl_TessLevelInner[0] = inner_ver; // 垂直
    gl_TessLevelInner[1] = inner_hori; // 水平
}