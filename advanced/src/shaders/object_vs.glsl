#version 460 core

struct DirectionalLight
{
	vec3 dirNormalized;
	vec3 diffuseColor; 
	vec3 specularColor;
};

struct PointLight
{
	vec3 pos; 
	vec3 diffuseColor; 
	vec3 specularColor;
	float attenConst, attenLin, attenQuad; 
};

struct ConeLight
{
	vec3 pos;
	vec3 dirNormalized; 
	vec3 diffuseColor; 
	vec3 specularColor;
	float angleCosine; 
	float attenConst, attenLin, attenQuad; 
};

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

out vsLightOut
{
	DirectionalLight TBNdirLight;
	PointLight TBNpointLight;
	vec3 TBNviewPos;
	vec3 TBNfragWorldCoord;
}
lightOut;

out vsShadowOut
{
	vec4 fragDirLightSpaceCoord;
	vec3 fragPointLightSpaceCoord;
}
shadowOut;

uniform mat4 u_dirLightSpaceMatrix;
uniform DirectionalLight u_dirLight;
uniform PointLight u_pointLight;
uniform vec3 u_viewPos;

out vec2 texCoord;

void main()
{
	texCoord = v_texCoord;
	
	vec3 fragWorldCoord = vec3(v_model * vec4(v_position, 1.0f));

	mat3 normalTransMatrix = mat3(transpose(inverse(v_model)));
	vec3 normal = normalTransMatrix * v_normal;
	vec3 tangent = normalTransMatrix * v_tangent;
	vec3 bitangent = cross(normal, tangent);
	mat3 inverseTBNMatrix = transpose(mat3(tangent, bitangent, normal));

	lightOut.TBNfragWorldCoord = inverseTBNMatrix * fragWorldCoord;
	lightOut.TBNviewPos = inverseTBNMatrix * u_viewPos;
	lightOut.TBNdirLight = u_dirLight;
	lightOut.TBNdirLight.dirNormalized = inverseTBNMatrix * u_dirLight.dirNormalized;
	lightOut.TBNpointLight = u_pointLight;
	lightOut.TBNpointLight.pos = inverseTBNMatrix * u_pointLight.pos;

	shadowOut.fragDirLightSpaceCoord = u_dirLightSpaceMatrix * vec4(fragWorldCoord, 1.0f);
	shadowOut.fragPointLightSpaceCoord = fragWorldCoord - u_pointLight.pos;

	gl_Position = u_projection * u_view * vec4(fragWorldCoord, 1.0f);
}