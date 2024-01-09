#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coord;
layout (location = 2) in vec3 aNormal;
out vec2 tc;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
layout (binding=0) uniform sampler2D s;

void main(void)
{	gl_Position = proj_matrix * mv_matrix * vec4(position,1.0);
	tc = tex_coord;
	FragPos = vec3(model * vec4(position, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;
}

