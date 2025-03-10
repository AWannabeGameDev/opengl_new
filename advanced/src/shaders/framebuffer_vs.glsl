#version 460 core

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec2 v_texCoord;

out vec2 texCoord;

void main()
{
	gl_Position = vec4(v_pos, 1.0f);
	texCoord = v_texCoord;
}