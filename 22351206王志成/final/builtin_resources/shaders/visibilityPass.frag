#version 460

#extension GL_GOOGLE_include_directive : enable

#include "include/packing.glsl"

layout(location = 0) flat in uint drawIndex;
layout(location = 0) out vec4 fragColor;

void main()
{
    fragColor = unpackUnorm4x8(((drawIndex & 255) << 23) | (gl_PrimitiveID & ((1 << 23) - 1)));
}