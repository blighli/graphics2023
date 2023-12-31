#version 330 core
uniform sampler2D texture1;
out vec4 frag_color;  
in vec4 out_color;
in vec2 uv;
in vec3 normal;

void main()
{
    vec4 tex_color = texture(texture1, uv);
    frag_color = out_color * tex_color;
    //frag_color = vec4(normal, 1.0);   
}