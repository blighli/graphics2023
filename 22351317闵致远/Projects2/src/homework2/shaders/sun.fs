#version 330 core
out vec4 FragColor;

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;

uniform sampler2D sun_texture; 

void main()
{    
    FragColor = texture(sun_texture, TexCoords);
}