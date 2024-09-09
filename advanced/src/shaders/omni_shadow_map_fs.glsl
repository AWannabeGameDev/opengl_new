#version 460 core

layout(std140) uniform pointLightFarPlane
{
	float u_pointLightFarPlane;
};

in vec3 fragViewCoord;

void main()
{
	gl_FragDepth = length(fragViewCoord) / u_pointLightFarPlane;
}