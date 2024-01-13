#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec3 normal;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

out vec2 tc;

layout(binding = 0) uniform sampler2D samp;

void main()
{
	gl_Position = proj_matrix * mv_matrix * vec4(position, 1.f);
	tc = texCoords;
}

