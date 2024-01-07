#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D Texture;

void main()
{
    FragColor = 0.3f * texture(Texture, TexCoords);
}