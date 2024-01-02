#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D light_texture;

void main()
{
    FragColor = texture(light_texture, TexCoords);
}