#version 330 core
uniform sampler2D texture1;
out vec4 frag_color;  
in vec4 out_color;
in vec2 uv;

void main()
{
    vec4 tex_color = texture(texture1, uv);
    frag_color = out_color * tex_color;
    
}