#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D Texture;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    // 环境光光强
    float ambientStrength = 0.1;
    // 添加环境光的颜色
    vec3 ambient = ambientStrength * lightColor;
    //-------------------------------------------------//
    // 顶点法向
    vec3 norm = normalize(Normal);
    // 光线方向：光源位置 - 物体顶点位置
    vec3 lightDir = normalize(lightPos - FragPos);
    // 漫反射光强（漫反射分量）
    float diff = max(dot(norm, lightDir), 0.0);
    // 添加漫反射光的颜色
    vec3 diffuse = diff * lightColor;
    //-------------------------------------------------//
    // 镜面反射光强，注意 1.0 会非常亮
    float specularStrength = 0.5;
    // 计算视线方向
    vec3 viewDir = normalize(viewPos - FragPos);
    // 计算反射光方向
    vec3 reflectDir = reflect(-lightDir, norm);
    // 计算镜面分量，32是物体产生高光的反光度
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    // 耦合各参数
    vec3 specular = specularStrength * spec * lightColor;
    //-------------------------------------------------//
    // 再乘上物体贴图颜色，得到最后可视的物体颜色
    vec3 result = ambient + diffuse + specular;
    FragColor = texture(Texture, TexCoord) * vec4(result, 1.0);
}