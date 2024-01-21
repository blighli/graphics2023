#ifndef _LAYOUT_H_
#define _LAYOUT_H_

struct VertexInput
{
	vec4 slot0;
	vec4 slot1;
	vec4 slot2;
};

struct VertexAttribute
{
	vec3 pos;
	vec3 normal;
	vec2 uv;
};

struct FaceAttribute
{
	vec3 normal;
	uint materialIndex;
};

struct MaterialAttribute
{
	vec3 ambient;
	uint ambientTexIndex;
	vec3 diffuse;
	uint diffuseTexIndex;
	vec3 specular;
	uint specularTexIndex;
	vec3 transmittance;
	float dissolve;
	vec3 emission;
	int illumModel;
	float shininess;
	float ior;
	uint normalTexIndex;
	uint displacementTexIndex;
	uint reflectionTexIndex;

	float roughness;
	float metallic;
	float sheen;
	float clearcoatThickness;
	float clearcoatRoughness;
	float anisotropy;
	float anisotropyRotation;
	uint basicPBRTexIndex;
	uint extraPBRTexIndex;

	vec2 _;
};

struct TriangleData
{
	VertexAttribute vertices[3];
	MaterialAttribute material;
	vec3 faceNormal;
};

struct RasterizedTriangle
{
	vec2 positionScreen[3];
	float positionDepth[3];
	mat3 matrixNormal;
	vec3 invW;
};

#endif