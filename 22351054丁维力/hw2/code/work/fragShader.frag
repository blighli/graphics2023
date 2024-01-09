#version 430

in vec2 tc;
in vec3 Normal;  
in vec3 FragPos;  

out vec4 color;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform vec3 lightPos;  
uniform float isSun;
layout (binding=0) uniform sampler2D s;

void main(void)
{	
if(isSun<=0)
{
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;
    ambient = ambient * vec3(texture(s, tc));//»·¾³¹â
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos); 
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    diffuse=diffuse*vec3(texture(s, tc));
    vec3 result = (ambient + diffuse) ;
    color = vec4(result,1.0);
    }
    else{
    color=texture(s, tc);
    }
}

