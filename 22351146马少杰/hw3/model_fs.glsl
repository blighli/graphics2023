#version 410 core

struct DirLight {  // 定义平行光结构体
    vec3 direction;  // 光照方向
    
    vec3 ambient;  // 环境光分量
    vec3 diffuse;  // 漫反射光分量
    vec3 specular;  // 镜面反射光分量
};

struct PointLight {  // 定义点光源结构体
    vec3 position;  // 光源位置
    
    float constant;  // 常数衰减因子
    float linear;  // 线性衰减因子
    float quadratic;  // 二次衰减因子
    
    vec3 ambient;  // 环境光分量
    vec3 diffuse;  // 漫反射光分量
    vec3 specular;  // 镜面反射光分量
};

struct SpotLight {  // 定义聚光灯结构体
    vec3 position;  // 光源位置
    vec3 direction;  // 光照方向
    float cutOff;  // 内切角
    float outerCutOff;  // 外切角
  
    float constant;  // 常数衰减因子
    float linear;  // 线性衰减因子
    float quadratic;  // 二次衰减因子
  
    vec3 ambient;  // 环境光分量
    vec3 diffuse;  // 漫反射光分量
    vec3 specular;  // 镜面反射光分量       
};


out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_specular1;

#define MAX_POINT_LIGHTS_NUM 8
uniform vec3 viewPos;
uniform int pointLightNum; 
uniform PointLight pointLights[MAX_POINT_LIGHTS_NUM];
uniform DirLight dirLight;
uniform SpotLight spotLight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff* vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos); 
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}
void main()
{    
    vec3 normal = texture(texture_normal1,TexCoords).rgb;  // 从法线贴图中获取法线向量
    normal = normalize(Normal);  // 将法线向量归一化
    
    vec3 viewDir = normalize(viewPos - FragPos);  // 计算视线方向

    vec3 result = CalcDirLight(dirLight, normal, viewDir);  // 计算平行光照射效果
    for(int i = 0; i < pointLightNum && i < MAX_POINT_LIGHTS_NUM; i++)
        result += CalcPointLight(pointLights[i], normal, FragPos, viewDir);  // 计算点光源照射效果    
    result += CalcSpotLight(spotLight, normal, FragPos, viewDir);  // 计算聚光灯照射效果    
    
    FragColor = vec4(result, 1.0);  // 将最终颜色输出
}


