#version 460

layout(location = 0) out vec4 vertexColor;

vec2 pos[3] = vec2[](
    vec2(.0f, -.5f),
    vec2(-.5f, .5f),
    vec2(.5f, .5f)
);

vec4 col[3] = vec4[](
    vec4(1.f, .0f, .0f, 1.f),
    vec4(.0f, 1.f, .0f, 1.f),
    vec4(.0f, .0f, 1.f, 1.f)
);

void main()
{
    gl_Position = vec4(pos[gl_VertexIndex], .0f, 1.f);
    vertexColor = col[gl_VertexIndex];
}