#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  

in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;
  
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
// 暂时不用该属性
uniform vec3 objectColor;

void main()
{
    // ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
        
    //vec3 result = (ambient + diffuse + specular) * objectColor;
    //FragColor = vec4(result, 1.0);
    vec4 texColor = texture(texture1, TexCoord); // 获取纹理的RGBA分量
    vec3 result = (ambient + diffuse + specular) * texColor.rgb; // 只将光照影响应用到RGB分量
    FragColor = vec4(result, texColor.a); // 使用纹理的alpha分量作为最终颜色的alpha值
    //FragColor = texture(texture1, TexCoord);
} 