#version 330 core
out vec4 FragColor;

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
    float shininess;
}; 
uniform Material material;
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform bool isSun;
void main()
{
    if(isSun == true){
        FragColor = texture(material.diffuse, TexCoords);  
    }
    else
    {
        // ambient
        vec3 ambient = 0.1 * texture(material.diffuse, TexCoords).rgb;

        // diffuse 
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = 1.0 * diff * texture(material.diffuse, TexCoords).rgb;  

        // specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = max(pow(dot(viewDir, reflectDir), material.shininess), 0.0);
        vec3 specular = 0.2 * spec * texture(material.specular, TexCoords).rgb;  

        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
    }
}