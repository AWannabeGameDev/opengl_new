#version 460 core

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec2 v_texCoord;
layout(location = 2) in mat4 v_model;
layout(location = 6) in vec3 v_normal;
layout(location = 7) in vec3 v_tangent;

layout(std140) uniform mats
{
	mat4 u_view;
	mat4 u_projection;
};

const int MAX_DIR_LIGHTS = 3;
layout(std140) uniform dirLightMatrices
{
	mat4 u_dirLightSpaceMatrices[MAX_DIR_LIGHTS];
};

layout(std140) uniform numDirLights
{
	int u_numDirLights;
};

out vec4 fragDirLightSpaceCoords[MAX_DIR_LIGHTS];

out vec2 texCoord;
out mat3 TBNMatrix;
out vec3 fragWorldCoord;

void main()
{
	fragWorldCoord = vec3(v_model * vec4(v_position, 1.0f));
	gl_Position = u_projection * u_view * vec4(fragWorldCoord, 1.0f);
	texCoord = v_texCoord;

	for(int dirLightIdx = 0; dirLightIdx < u_numDirLights; dirLightIdx++)
	{
		fragDirLightSpaceCoords[dirLightIdx] = u_dirLightSpaceMatrices[dirLightIdx] * vec4(fragWorldCoord, 1.0f);
	}

	vec3 normal = normalize(vec3(v_model * vec4(v_normal, 0.0f)));
	vec3 tangent = normalize(vec3(v_model * vec4(v_tangent, 0.0f)));
	vec3 bitangent = cross(normal, tangent);
	TBNMatrix = mat3(tangent, bitangent, normal);
}