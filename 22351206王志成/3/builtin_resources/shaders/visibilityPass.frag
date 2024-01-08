#version 460

#extension GL_GOOGLE_include_directive : enable

#include "include/packing.glsl"

layout(location = 0) flat in uint drawIndex;
layout(location = 1) in vec3 pos;
layout(location = 2) in vec3 normal;

layout(push_constant) uniform PushConstants 
{
    mat4 matrixModel;
    mat4 matrixVP; 
    vec4 lightProperty;
    uint lightType;
};

layout(location = 0) out vec4 fragColor;

void main()
{
    fragColor = unpackUnorm4x8(((drawIndex & 255) << 23) | (gl_PrimitiveID & ((1 << 23) - 1)));
    if(lightType == 0) fragColor *= dot(normal, normalize(lightProperty.xyz)) * lightProperty.w;
    else fragColor *= dot(normal, normalize(lightProperty.xyz - pos)) * lightProperty.w / (length(lightProperty.xyz - pos) * length(lightProperty.xyz - pos));
}