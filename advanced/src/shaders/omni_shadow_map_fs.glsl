#version 460 core

uniform float u_farPlane;

in vec3 fragViewCoord;

void main()
{
	gl_FragDepth = length(fragViewCoord) / u_farPlane;
}