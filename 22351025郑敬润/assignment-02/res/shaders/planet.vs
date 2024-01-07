#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{	
	// 定点位置
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	// 贴图位置
	TexCoord = aTexCoord;
	// 计算世界坐标系中的顶点位置，传给片段着色器
	FragPos = vec3(model * vec4(aPos, 1.0));
	// 将顶点数据中包含的法向数据传给片段着色器
	// 此处使用法线矩阵来移除对法向量错误缩放的影响，仅供学习中使用，求逆运算开销很大！！！
	Normal = mat3(transpose(inverse(model))) * aNormal; 
}