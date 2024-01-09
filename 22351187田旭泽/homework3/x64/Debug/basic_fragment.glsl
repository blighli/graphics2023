#version 460 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;


uniform sampler2D texTure;


void main()
{
   FragColor = texture(texTure, TexCoord);
   
   vec3 lightDir = normalize(vec3(0.f, 0.f, 0.f) - FragPos);
   float coeff = max(dot(lightDir, normalize(Normal)), 0.1f);
   FragColor *= coeff;
}