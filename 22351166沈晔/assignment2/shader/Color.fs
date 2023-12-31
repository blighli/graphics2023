// Blinn Phong Fragment Shader
#version 450 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

struct Light {
    vec3 Position;
    vec3 Color;
    float intensity;
};

uniform Light light_list[20];
uniform int num_lights;
uniform sampler2D texture_diffuse1;
uniform vec3 viewPos;

void main()
{
    vec4 colorv4 = texture(texture_diffuse1, vs_out.TexCoords);
    vec3 color = vec3(colorv4);

    vec3 normal = normalize(vs_out.Normal);
    // lighting
    vec3 lighting = vec3(0.0);
    vec3 viewDir = normalize(viewPos - vs_out.FragPos);

    for(int i = 0; i < num_lights; i++)
    {
        // diffuse
        vec3 lightDir = normalize(light_list[i].Position - vs_out.FragPos);
        float diff = max(dot(lightDir, normal), 0.0);

        //specular
        vec3 halfvec=normalize(viewDir+lightDir);
        float dot_spec=max(dot(halfvec, normal), 0.0);
        float specular=pow(dot_spec,64);

        vec3 result = light_list[i].Color * diff * color + vec3(specular);
        // attenuation (use quadratic as we have gamma correction)
        float distance = length(vs_out.FragPos - light_list[i].Position);
        result *= 1.0 / (distance * distance);

        result *= light_list[i].intensity;
        lighting += result;
    }

    vec3 result=lighting + color * 0.2;

    FragColor = vec4(result, 1.0);
}