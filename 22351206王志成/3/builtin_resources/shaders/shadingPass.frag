#version 460

layout(set = 1, binding = 0) uniform sampler2D visibilityBuffer;
layout(set = 1, binding = 1) uniform sampler2D depthBuffer;

layout(location = 0) in vec2 texCoords;
layout(location = 0) out vec4 fragColor;

void main()
{
    fragColor = texture(visibilityBuffer, texCoords);
    // fragColor = vec4(texCoords, 0, 1);
}