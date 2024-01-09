#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 in_uv;
layout (location = 3) in vec3 in_normal;
layout (location = 4) in vec3 in_tangent;
layout (location = 5) in vec3 in_bitangent;

out vec4 out_color;
out vec2 uv;
out vec3 out_normal;
out vec3 frag_pos;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0);
    out_color = color;
    uv = in_uv;
    out_normal = in_normal;
    frag_pos = vec3(model * vec4(pos, 1.0f));

    vec3 temp_n = cross(in_tangent, in_bitangent);
    vec3 t = normalize(vec3(model * vec4(in_tangent, 0.0f)));
    vec3 b = normalize(vec3(model * vec4(in_bitangent, 0.0f)));
    vec3 n = normalize(vec3(model * vec4(temp_n, 0.0f)));
    TBN = mat3(t, b, n);
    //out_color = vec4(1.0, 0.0, 0.0, 1.0);
    //out_color = color;
}