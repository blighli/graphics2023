#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D u_Texture;

void main()
{
    // 太阳作为点光源的光照计算
    vec3 lightColor = vec3(1.0, 1.0, 1.0); // 光源颜色
    vec3 lightPos = vec3(0.0, 1.0, 0.0);   // 光源位置

    // 计算光线方向
    vec3 lightDir = normalize(lightPos - vec3(0.0, 0.0, 0.0));
    // 计算法向量
    vec3 normal = normalize(cross(dFdx(TexCoord), dFdy(TexCoord)));

    // 计算漫反射
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // 采样纹理
    vec4 texColor = texture(u_Texture, TexCoord);

    // 最终颜色 = 漫反射颜色 * 纹理颜色
    FragColor = vec4(diffuse, 1.0) * texColor;
}
