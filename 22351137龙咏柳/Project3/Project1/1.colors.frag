#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform vec3 lightPos;
uniform vec3 lightPos_pro;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform vec3 objectColor;
uniform sampler2D texture1;


void main()
{
    // ambient
    float ambientStrength = 0.12;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);//入射方向
	vec3 lightDir_pro = normalize(lightPos_pro - FragPos);//入射方向

    float diff = max(dot(norm, lightDir), 0.0);
	float diff_pro = max(dot(norm, lightDir_pro), 0.0);

    vec3 diffuse = diff * lightColor;
	vec3 diffuse_pro = diff_pro * lightColor;

	// specular
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	vec3 reflectDir_pro = reflect(-lightDir_pro, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
	float spec_pro = pow(max(dot(viewDir, reflectDir_pro), 0.0), 128);

	vec3 specular = specularStrength * lightColor * spec;
	vec3 specular_pro = specularStrength * lightColor * spec_pro;


    vec3 result = (ambient + diffuse + specular + diffuse_pro + specular_pro) * objectColor;
	FragColor = texture(texture1, TexCoord)*vec4(result, 1.0);
} 