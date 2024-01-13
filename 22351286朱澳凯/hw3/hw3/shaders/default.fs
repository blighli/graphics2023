#version 330 core

#define LIGHT_NUM 10

out vec4 FragColor;
in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light lights[LIGHT_NUM];
uniform sampler2D texture1;
uniform int lightNum;
uniform vec3 viewPos; 
void main()
{
    FragColor = vec4(0,0,0,0);
    for(int i = 0; i < lightNum; ++i)
    {
        Light light = lights[i];
        vec3 ambient = light.ambient * texture(texture1, TexCoords).rgb;

        // diffuse 
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(light.position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * texture(texture1, TexCoords).rgb;  

        // specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = max(pow(dot(viewDir, reflectDir), 64), 0.0);
        vec3 specular = light.specular * spec * texture(texture1, TexCoords).rgb;  

        vec3 result = ambient + diffuse + specular;
        FragColor.xyz += result.xyz;
    }
}
