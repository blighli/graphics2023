#version 330 core
uniform sampler2D texture1;
uniform vec3 light_pos;
uniform vec3 view_pos;
uniform vec3 light_color;
uniform vec3 sheld_color;
out vec4 frag_color;  
in vec4 out_color;
in vec2 uv;
in vec3 normal;
in vec3 frag_pos;

void main()
{
    // ambient
    float ambient_strength = 0.1;
    vec3 ambient = ambient_strength * light_color;
  	
    // diffuse 
    vec3 norm = normalize(normal);
    vec3 light_dir = normalize(light_pos - frag_pos);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light_color;
    
    // specular
    float specular_strength = 0.5;
    vec3 view_dir = normalize(view_pos - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, norm);  
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular = specular_strength * spec * light_color;  
        
    vec4 tex_color = texture(texture1, uv);
    vec4 object_color = out_color * tex_color;

    vec4 result = vec4(ambient + diffuse + specular, 1.0f) * object_color * vec4(sheld_color, 1.0f);
    frag_color = result;
    //frag_color = object_color * vec4(sheld_color, 1.0f);

    //frag_color = vec4(normal, 1.0);   
}