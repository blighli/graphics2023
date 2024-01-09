#version 410 core

// 顶点属性
layout (location = 0) in vec3 aPos; // 顶点位置
layout (location = 1) in vec3 aNormal; // 顶点法线
layout (location = 2) in vec2 aTexCoords; // 纹理坐标
layout (location = 3) in vec3 tangent; // 切线
layout (location = 4) in vec3 bitangent; // 双切线

// 输出变量
out vec2 TexCoords; // 传递给片段着色器的纹理坐标
out vec3 FragPos; // 片段的位置（在世界坐标系中）
out vec3 Normal; // 片段的法线（在世界坐标系中）

// 统一变换矩阵
uniform mat4 model; // 模型矩阵，将物体从模型空间变换到世界空间
uniform mat4 cameraMatrix; // 相机矩阵，将世界坐标系中的点变换到相机坐标系中
uniform mat4 perspectiveMatrix; // 透视投影矩阵，将相机坐标系中的点变换到裁剪空间

void main()
{
    // 计算顶点在裁剪空间中的位置
    gl_Position = perspectiveMatrix * cameraMatrix * model * vec4(aPos, 1.0);

    // 传递纹理坐标给片段着色器
    TexCoords = aTexCoords;

    // 将顶点位置变换到世界坐标系中
    FragPos = vec3(model * vec4(aPos, 1.0));

    // 传递顶点法线给片段着色器
    Normal = aNormal;
}
