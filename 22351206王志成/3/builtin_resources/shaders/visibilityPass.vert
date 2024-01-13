#version 460

#extension GL_ARB_shader_draw_parameters : enable

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(push_constant) uniform PushConstants 
{
    mat4 matrixModel;
    mat4 matrixVP; 
    vec4 lightProperty;
    uint lightType;
};

layout(location = 0) flat out uint drawIndex;
layout(location = 1) out vec3 _pos;
layout(location = 2) out vec3 _normal;

void main()
{
    gl_Position = matrixVP * matrixModel * vec4(pos, 1.0);
    drawIndex = gl_DrawIDARB;
    _pos = (matrixVP * matrixModel * vec4(pos, 1.0)).xyz;
    _normal = normalize(matrixModel * vec4(normal, 0.0)).xyz;
}