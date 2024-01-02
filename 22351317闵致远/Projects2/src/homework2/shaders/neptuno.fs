#version 330 core
out vec4 FragColor;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
  
uniform vec3 viewPos;
uniform Light light;

uniform sampler2D neptuno_texture; 

void main()
{    
    vec3 texture_rgb = texture(neptuno_texture, TexCoords).rgb;
    // ambient
    vec3 ambient = light.ambient * texture_rgb;

    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture_rgb;

    // // specular
    // vec3 viewDir = normalize(viewPos - FragPos);
    // vec3 reflectDir = reflect(-lightDir, norm);  
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0f);
    // vec3 specular = light.specular * spec * texture(texture_specular1, TexCoords).rgb;  

    // attenuation
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    ambient  *= attenuation;  
    diffuse   *= attenuation;
    // specular *= attenuation;

    // vec3 result = ambient + diffuse + specular;
    vec3 result = ambient + diffuse;
    FragColor = vec4(result, 1.0f);
    // FragColor = texture(texture_diffuse1, TexCoords);
}