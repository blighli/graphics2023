#version 460

layout(binding = 0) uniform sampler2D earthTexture;
layout(binding = 1) uniform sampler2D moonTexture;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 fragColor;

void main()
{
    fragColor = vec4(1, 0, 0, 0);
}