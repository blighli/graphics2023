#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;

void main(){
    //光照颜色和材质颜色
    vec3 lightColor = vec3(1.0f, 0.85f, 0.2f);
//     vec3 lightPos =  vec3(0.0f,100.0f,0.0f);
        vec3 lightPos =  vec3(-1.0f,7.0f,1.0f);
    vec3 objectColor = vec3(texture(texture_diffuse1, TexCoords));

    float ambientRatio = 0.01f;
    vec3 ambient = ambientRatio * lightColor ;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);

    vec3 diffuse = diff * lightColor;
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, texture(texture_diffuse1, TexCoords).a);
}