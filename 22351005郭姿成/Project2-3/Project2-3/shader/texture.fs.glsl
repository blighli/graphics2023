#version 410 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D Texture;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform mat4 frag_view;
uniform mat4 frag_model;
uniform vec3 lightColor;
uniform Material material;

void main() {
    vec4 Objectcolor = texture(Texture, TexCoord);
    // ambient
    float ambientStrength = 1.0;
    vec3 ambient = ambientStrength * lightColor * material.ambient;

    // diffuse
    // 固定光源
    vec3 fNormal = mat3(transpose(inverse(frag_model))) * Normal;
    vec3 norm = normalize(fNormal);
    vec3 LightPos = vec3(frag_view * vec4(lightPos, 1.0));
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * material.diffuse) * lightColor;

    // specular---Blinn-Phong模型
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfDir, norm), 0.0), material.shininess);
    vec3 specular = specularStrength * (spec * material.specular) * lightColor;

    // 组合得到结果
    vec3 result = (ambient + diffuse + specular) * vec3(Objectcolor);
    FragColor = vec4(result, 1.0);
}