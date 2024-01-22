#version 460

#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_GOOGLE_include_directive : enable

#include "include/layout.glsl"

layout(set = 0, binding = 0) restrict readonly buffer VertexAttributes { VertexInput vertices[]; };
layout(set = 0, binding = 1) restrict readonly buffer FaceAttributes { FaceAttribute faces[]; };
layout(set = 0, binding = 2) restrict readonly buffer MaterialAttributes { MaterialAttribute materials[]; };
layout(set = 0, binding = 3) restrict readonly buffer IndexAttributes { uint indices[]; };
layout(set = 1, binding = 0) uniform sampler2D visibilityBuffer;
layout(set = 1, binding = 1) uniform sampler2D depthBuffer;
layout(set = 2, binding = 0) uniform sampler2D textures[];

layout(push_constant) uniform PushConstants 
{
    mat4 matrixModel;
    mat4 matrixView;
	mat4 matrixProj;
	float nearClip;
	float farClip;
	vec2 _;
	vec3 lightDirection;
	float lightIntensity;
};

layout(location = 0) in vec2 texCoords;
layout(location = 0) out vec4 fragColor;

#include "include/packing.glsl"
#include "include/common.glsl"

void main()
{
	const uint unpackedIndices = packUnorm4x8(texture(visibilityBuffer, texCoords));
	if(unpackedIndices == 0U) discard;
	const bool isAlpha = bool(unpackedIndices & 0x80000000);
	const uint instanceIndex = (unpackedIndices >> 23) & 255;
	const uint primitiveIndex = unpackedIndices & ((1 << 23) - 1);

	TriangleData data;
	data.vertices[0] = unpackVertexData(vertices[indices[3 * primitiveIndex + 0]]);
	data.vertices[1] = unpackVertexData(vertices[indices[3 * primitiveIndex + 1]]);
	data.vertices[2] = unpackVertexData(vertices[indices[3 * primitiveIndex + 2]]);
	data.material = materials[faces[primitiveIndex].materialIndex];
	data.faceNormal = faces[primitiveIndex].normal;

	RasterizedTriangle rasterData = rasterization(data, matrixModel, matrixView, matrixProj);
	vec3 barycentricCoords = calBarycentricCoords(texCoords, rasterData.positionScreen);
	vec4 positionWorld = convertScreenPositionToWorldPosition(texCoords, barycentricCoords, rasterData.invW, matrixView, matrixProj);
	vec4 positionCamera = calCameraPosition(matrixView);

	vec2 uv = perspectiveCorrectBarycentricInterpolation(vec2[3](data.vertices[0].uv, data.vertices[1].uv, data.vertices[2].uv), barycentricCoords, rasterData.invW);

	vec3 normalWorld = (transpose(inverse(matrixModel)) * vec4(data.faceNormal, 0)).xyz;
	normalWorld = normalize(normalWorld);
	vec3 dirLight = normalize(lightDirection);
	float LdotN = max(dot(normalWorld, -dirLight), 0);
	vec3 dirRef = reflect(dirLight, normalWorld);
	vec3 dirView = normalize(positionWorld - positionCamera).xyz;
	float RdotV = max(dot(dirRef, dirView), 0);

	vec4 outColor = vec4(data.material.diffuse, 1);
    outColor = (data.material.diffuseTexIndex != 0x7FFFFFFF) ? 
			   texture(textures[nonuniformEXT(data.material.diffuseTexIndex)], uv) * (length(outColor) > 0 ? outColor : vec4(1))
			   : outColor;
	outColor *= LdotN;
	outColor += vec4(data.material.specular, 1) * PhongNormalDistribution(RdotV, 1, data.material.shininess);
	outColor *= lightIntensity;
	outColor += vec4(0.17f, 0.37f, 0.65f, 1) * .1f;

	fragColor = outColor;
}