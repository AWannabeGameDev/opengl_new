#version 460 core

in vec3 cubemapTexCoord;

uniform samplerCube u_skybox;

out vec4 fragColor;

void main()
{
	fragColor = texture(u_skybox, cubemapTexCoord) * 0.3f;
}