#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D sprite;
uniform vec4 spriteColor;

void main()
{
	color = spriteColor * texture(sprite, TexCoords);
	//color=vec4(0.3,0.7,0.5,1.0);
}