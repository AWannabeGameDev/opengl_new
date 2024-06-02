#version 460 core

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec2 v_texCoord;
layout(location = 2) in mat4 v_model;
layout(location = 6) in vec3 v_normal;

layout(std140) uniform mats
{
	mat4 u_view;
	mat4 u_projection;
};

uniform mat4 u_dirLightSpaceMatrix;

out vec4 fragDirLightSpaceCoord;

out vec2 texCoord;
out vec3 normal;
out vec3 fragWorldCoord;

void main()
{
	fragWorldCoord = vec3(v_model * vec4(v_position, 1.0f));
	gl_Position = u_projection * u_view * vec4(fragWorldCoord, 1.0f);
	texCoord = v_texCoord;
	normal = mat3(transpose(inverse(v_model))) * v_normal;
	fragDirLightSpaceCoord = u_dirLightSpaceMatrix * vec4(fragWorldCoord, 1.0f);
}