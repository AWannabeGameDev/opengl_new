#version 460 core

layout(location = 0) in vec3 v_pos;
layout(location = 2) in mat4 v_model;

void main()
{
	gl_Position = v_model * vec4(v_pos, 1.0f);
}