#version 330 core
#define MAX_LIGHT_NR 4
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;
uniform vec3 view_pos;

struct Point_Light
{
    vec3 position;
    vec3 color;
};

uniform Point_Light point_lights[MAX_LIGHT_NR];

out vec4 frag_color;  
in vec4 out_color;
in vec2 uv;
in mat3 TBN;
in vec3 frag_pos;
in vec3 out_normal;

vec3 calc_point_light(Point_Light light, vec3 normal, vec3 frag_pos, vec3 view_dir);

void main()
{
    vec3 normal_tbn = texture(texture_normal1, uv).rgb;
    normal_tbn = normalize(normal_tbn * 2.0 - 1.0);
    //vec3 normal = normalize(TBN * normal_tbn);
    vec3 normal = normalize(out_normal);
    vec3 result = vec3(0.0f, 0.0f, 0.0f);
    vec3 view_dir = normalize(view_pos - frag_pos);
    for (int i = 0; i < MAX_LIGHT_NR; i++) {
        result += calc_point_light(point_lights[i], normal, frag_pos, view_dir);
    }
        
    frag_color = vec4(result, 1.0f);
       
}

vec3 calc_point_light(Point_Light light, vec3 normal, vec3 frag_pos, vec3 view_dir)
{
    vec3 light_dir = normalize(light.position - frag_pos);
    // 漫反射着色
    float diff = max(dot(normal, light_dir), 0.0);
    // 镜面光着色
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    // 衰减
    float distance    = length(light.position - frag_pos);
    float attenuation = 1.0;    
    // 合并结果
    vec3 ambient  = light.color * vec3(texture(texture_diffuse1, uv)) * 0.3f;
    vec3 diffuse  = light.color * diff * vec3(texture(texture_diffuse1, uv));
    vec3 specular = light.color * spec * vec3(texture(texture_specular1, uv));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}