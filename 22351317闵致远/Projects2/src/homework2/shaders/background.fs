#version 330 core
out vec4 FragColor;

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;

uniform sampler2D background_texture; 

void main()
{    
    FragColor = texture(background_texture, TexCoords);
}