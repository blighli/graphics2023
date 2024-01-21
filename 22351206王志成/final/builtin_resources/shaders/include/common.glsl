#ifndef _COMMON_H_
#define _COMMON_H_

#include "layout.glsl"

float convertToLinearDepth(in float depth, in float near, in float far)
{
    return (2.0 * near) / (near + far - depth * (far - near));
}

RasterizedTriangle rasterization(in TriangleData data, in mat4 model, in mat4 view, in mat4 proj)
{
    const mat4 mvp = proj * view * model;
    mat3x4 matNDC = mvp * mat3x4(vec4(data.vertices[0].pos, 1), vec4(data.vertices[1].pos, 1), vec4(data.vertices[2].pos, 1));
    matNDC[0][3] = 1 / matNDC[0][3];
    matNDC[1][3] = 1 / matNDC[1][3];
    matNDC[2][3] = 1 / matNDC[2][3];
    matNDC[0].xyz *= matNDC[0].w;
    matNDC[1].xyz *= matNDC[1].w;
    matNDC[2].xyz *= matNDC[2].w;

    RasterizedTriangle output_;
    output_.positionScreen = vec2[](matNDC[0].xy * .5f + .5f, matNDC[1].xy * .5f + .5f, matNDC[2].xy * .5f + .5f);
    output_.positionDepth = float[](matNDC[0].z, matNDC[1].z, matNDC[2].z);
    output_.invW = vec3(matNDC[0][3], matNDC[1][3], matNDC[2][3]);

    const mat4 transInvMVP = transpose(inverse(mvp));
    output_.matrixNormal = mat3((transInvMVP * vec4(data.vertices[0].normal, 0)).xyz,
                                (transInvMVP * vec4(data.vertices[1].normal, 0)).xyz,
                                (transInvMVP * vec4(data.vertices[2].normal, 0)).xyz);
    
    return output_;
}

vec3 calBarycentricCoords(in vec2 positionScreen, in vec2 triangleVerticesScreen[3])
{
    const vec3 v0 = vec3(triangleVerticesScreen[0] - positionScreen, 0);
    const vec3 v1 = vec3(triangleVerticesScreen[1] - positionScreen, 0);
    const vec3 v2 = vec3(triangleVerticesScreen[2] - positionScreen, 0);

    const float area1 = length(cross(v0, v1));
    const float area2 = length(cross(v1, v2));
    const float area3 = length(cross(v2, v0));
    const vec3 res = vec3(area2, area3, area1);
    return res / (area1 + area2 + area3);
}

vec4 convertScreenPositionToWorldPosition(in vec2 positionScreen, in vec3 barycentricCoords, in vec3 invW, in mat4 view, in mat4 proj)
{
    float w = 1 / dot(invW, barycentricCoords);
    float z = proj[2][2] * -w + proj[3][2];
    vec4 pos = inverse(proj * view) * vec4(positionScreen * w, z, w);
    return pos / pos.w;
}

vec4 calCameraPosition(in mat4 view)
{
    return inverse(view) * vec4(0, 0, 0, 1);
}

float perspectiveCorrectBarycentricInterpolation(in vec3 attributes, in vec3 barycentricCoords, in vec3 invW)
{
    float denom = 1 / dot(barycentricCoords, invW);
    return dot(attributes * barycentricCoords, invW) * denom;
}

vec2 perspectiveCorrectBarycentricInterpolation(in vec2 attributes[3], in vec3 barycentricCoords, in vec3 invW)
{
    mat2x3 matAttri = transpose(mat3x2(attributes[0], attributes[1], attributes[2]));
    float denom = 1 / dot(barycentricCoords, invW);
    return vec2(dot(matAttri[0] * barycentricCoords, invW), dot(matAttri[1] * barycentricCoords, invW)) * denom;
}

vec3 perspectiveCorrectBarycentricInterpolation(in vec3 attributes[3], in vec3 barycentricCoords, in vec3 invW)
{
    mat3 matAttri = transpose(mat3(attributes[0], attributes[1], attributes[2]));
    float denom = 1 / dot(barycentricCoords, invW);
    return vec3(dot(matAttri[0] * barycentricCoords, invW), dot(matAttri[1] * barycentricCoords, invW), dot(matAttri[2] * barycentricCoords, invW)) * denom;
}

vec4 perspectiveCorrectBarycentricInterpolation(in vec4 attributes[3], in vec3 barycentricCoords, in vec3 invW)
{
    mat4x3 matAttri = transpose(mat3x4(attributes[0], attributes[1], attributes[2]));
    float denom = 1 / dot(barycentricCoords, invW);
    return vec4(dot(matAttri[0] * barycentricCoords, invW), dot(matAttri[1] * barycentricCoords, invW), dot(matAttri[2] * barycentricCoords, invW), dot(matAttri[3] * barycentricCoords, invW)) * denom;
}

float PhongNormalDistribution(float RdotV, float intensity, float power) 
{
    float Distribution = pow(RdotV, power) * intensity;
    Distribution *= (2 + intensity) / (2 * 3.1415926535);
    return Distribution;
}

#endif