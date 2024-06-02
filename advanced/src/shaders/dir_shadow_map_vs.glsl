#version 460 core

layout(location = 0) in vec3 v_pos;
layout(location = 2) in mat4 v_model;

uniform mat4 u_lightSpaceMatrix;

void main()
{
	gl_Position = u_lightSpaceMatrix * v_model * vec4(v_pos, 1.0f);
}