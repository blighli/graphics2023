#version 330 core
out vec4 FragColor; 
in vec3 ourColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
uniform vec3 viewPos;
uniform vec3 lightPos;
uniform sampler2D Texture1;
uniform sampler2D Texture2;
uniform vec3 objectColor;
uniform vec3 lightColor;
//uniform sampler2D ourTexture;
//uniform vec4  ourColor;
void main()
{
    float specularStrength = 0.5;
    
    vec3 norm = normalize(Normal);

    vec3 lightDir = normalize(lightPos - FragPos);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    FragColor = vec4((0.1 + diffuse + specular),1.0) * mix(texture(Texture1, TexCoord), texture(Texture2, TexCoord), 1);
}