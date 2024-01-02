#version 460

layout(location = 0) in vec4 diffuseColor;
layout(location = 1) in vec4 worldPos;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec4 emission;
layout(location = 0) out vec4 fragColor;

void main()
{
    fragColor = clamp(diffuseColor * dot(normal, normalize(-worldPos.xyz)) * 100.f / (length(worldPos.xyz) * length(worldPos.xyz)), 0, 1) + emission;
}