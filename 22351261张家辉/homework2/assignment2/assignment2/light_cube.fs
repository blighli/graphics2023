#version 330 core
in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;

out vec4 FragColor;

void main()
{
    FragColor = vec4(0.8) * texture(texture1, TexCoord); // set all 4 vector values to 1.0
}