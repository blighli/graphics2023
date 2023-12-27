#version 410 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};


in vec2 TexCoords;
in vec3 FragPos;
in vec3 FragNormal;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform mat4 fragView;
uniform Material material;

void main() {
    // 环境光成分
    float ambientStrength = 0.7;
	vec3 ambient = ambientStrength * lightColor * material.ambient;

    // 漫反射光成分 此时需要光线方向为指向光源
    vec3 norm = normalize(FragNormal);
    vec3 LightPos = vec3(fragView * vec4(lightPos, 1.0));
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * material.diffuse) * lightColor;

    // 镜面反射成分 此时需要光线方向为由光源指出
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfDir, norm), 0.0), 64.0);
    vec3 specular = specularStrength * (spec * material.specular) * lightColor;

    // 组合得到结果
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}