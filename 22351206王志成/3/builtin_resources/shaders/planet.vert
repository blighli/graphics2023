#version 460

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 0) out vec4 diffuseColor;
layout(location = 1) out vec4 worldPos;
layout(location = 2) out vec3 _normal;
layout(location = 3) out vec4 emission;

layout(binding = 0) uniform ModelMatrices { mat4 model[3]; };
layout(binding = 1) uniform sampler2D earthTexture;
layout(binding = 2) uniform sampler2D moonTexture;
layout(binding = 3) uniform sampler2D sunTexture;

layout(push_constant) uniform PushConstants { mat4 matrixVP; };

void main()
{
    gl_Position = matrixVP * model[gl_InstanceIndex] * vec4(pos, 1.0);
    vec2 correctCoord = texCoord;
    correctCoord.x = 1 - correctCoord.x;
    if(gl_InstanceIndex == 0) { diffuseColor = texture(sunTexture, correctCoord); emission = diffuseColor; }
    if(gl_InstanceIndex == 1) { diffuseColor = texture(earthTexture, correctCoord); emission = diffuseColor * 0.1; }
    if(gl_InstanceIndex == 2) { diffuseColor = texture(moonTexture, correctCoord); emission = diffuseColor * 0.25; }
    worldPos = model[gl_InstanceIndex] * vec4(pos, 1.0);
    _normal = normalize(model[gl_InstanceIndex] * vec4(normal, 0)).xyz;
}