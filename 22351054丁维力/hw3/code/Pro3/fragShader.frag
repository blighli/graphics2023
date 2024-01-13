#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 normalIn;
in vec3 FragPosIn;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform vec3 lightPos2;
uniform vec3 lightColor2;
uniform vec3 lightPos3;
uniform vec3 lightColor3;
uniform vec3 lightPos4;
uniform vec3 lightColor4;
uniform sampler2D texture_diffuse1;

void main()
{    
	float ambientStrength = 0.05f;
	vec3 ambient = ambientStrength * vec3(texture(texture_diffuse1, TexCoords));
	vec3 norm = normalize(normalIn);
	vec3 lightDir = normalize(lightPos - FragPosIn);
	float diff1 = max(dot(norm, lightDir), 0.0f);
	// vec3 diffuse1 = diff1* vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse1 = diff1/5 * vec3(texture(texture_diffuse1, TexCoords))*0+diff1/5 * vec3(texture(texture_diffuse1, TexCoords))*lightColor*5;
	//light2
	vec3 lightDir2 = normalize(lightPos2 - FragPosIn);
	float diff2 = max(dot(norm, lightDir2), 0.0f);
	vec3 diffuse2 = diff2/5 * vec3(texture(texture_diffuse1, TexCoords))*0+diff2/5 * vec3(texture(texture_diffuse1, TexCoords))*lightColor2*5;
	//light3
	vec3 lightDir3 = normalize(lightPos3 - FragPosIn);
	float diff3 = max(dot(norm, lightDir3), 0.0f);
	//vec3 diffuse2 = diff2* vec3(texture(texture_diffuse1, TexCoords));
	vec3 diffuse3 = diff3/5 * vec3(texture(texture_diffuse1, TexCoords))*0+diff3/5 * vec3(texture(texture_diffuse1, TexCoords))*lightColor3*5;
	//light4
	vec3 lightDir4 = normalize(lightPos4 - FragPosIn);
	float diff4 = max(dot(norm, lightDir4), 0.0f);
	//vec3 diffuse2 = diff2* vec3(texture(texture_diffuse1, TexCoords));
	vec3 diffuse4= diff4/5 * vec3(texture(texture_diffuse1, TexCoords))*0+diff4/5 * vec3(texture(texture_diffuse1, TexCoords))*lightColor4*5;

	vec3 result = ambient + diffuse1+diffuse2+diffuse3+diffuse4 ;

    FragColor = vec4(result, 1.0f);

}