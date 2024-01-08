#version 460

layout(location = 0) out vec2 texCoords;

// use a larger triangle mesh to cover screen quad
// then regions outside screen quad will be culled
void main()
{
	texCoords = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	gl_Position = vec4(texCoords * 2 - 1, 1.f, 1.f);
}