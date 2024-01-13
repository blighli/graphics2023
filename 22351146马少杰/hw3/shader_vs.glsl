#version 410 core

layout(location = 0) in vec3 aPos;

uniform mat4 cameraMatrix; // 相机变换矩阵
uniform mat4 perspectiveMatrix; // 透视投影矩阵
uniform mat4 modelviewMatrix = mat4(1.f); // 模型视图变换矩阵

void main()
{
  // 计算最终的裁剪空间坐标，先进行模型、视图和投影变换
  gl_Position = perspectiveMatrix * cameraMatrix * modelviewMatrix * vec4(aPos, 1.0);
}