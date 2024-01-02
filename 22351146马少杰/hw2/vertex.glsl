#version 410

layout(location = 0) in vec3 VertexPosition; // 顶点位置
layout(location = 1) in vec3 VertexNormal; // 顶点法线
layout(location = 2) in vec2 UV; // 顶点的 UV 坐标

out vec3 FragNormal; // 片段法线
out vec2 FragUV; // 输出片段 UV 坐标
out vec4 spacePos; // 输出世界空间位置

uniform mat4 cameraMatrix; // 相机变换矩阵
uniform mat4 perspectiveMatrix; // 透视投影矩阵
uniform mat4 modelviewMatrix = mat4(1.f); // 模型视图变换矩阵

void main()
{
    // 计算经过模型视图变换后的顶点法线，并进行归一化
    FragNormal = normalize((modelviewMatrix * vec4(VertexNormal, 0.f)).xyz);
    FragUV = UV; // 将顶点的 UV 坐标传递到片段着色器

    spacePos = modelviewMatrix * vec4(VertexPosition, 1.0); // 计算世界空间中的顶点位置

    // 计算最终的裁剪空间坐标，先进行模型、视图和投影变换
    gl_Position = perspectiveMatrix * cameraMatrix * modelviewMatrix * vec4(VertexPosition, 1.0);
}
