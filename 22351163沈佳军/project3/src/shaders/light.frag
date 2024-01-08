#version 330 core

//in vec3 ourColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D ourTexture;


void main()
{
//    FragColor = vec4(ourColor, 1.0f);
//    FragColor = vec4(1.0f);
    FragColor = texture(ourTexture, TexCoords);
}