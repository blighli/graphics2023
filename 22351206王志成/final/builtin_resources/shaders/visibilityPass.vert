#version 460

#extension GL_ARB_shader_draw_parameters : enable

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(push_constant) uniform PushConstants 
{
    mat4 matrixModel;
    mat4 matrixView;
    mat4 matrixProj; 
};

layout(location = 0) flat out uint drawIndex;

void main()
{
    gl_Position = matrixProj * matrixView * matrixModel * vec4(pos, 1.0);
    drawIndex = gl_DrawIDARB;
}