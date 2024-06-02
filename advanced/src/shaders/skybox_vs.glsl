#version 460 core

layout(location = 0) in vec3 v_pos;

layout(std140) uniform mats
{
	mat4 u_view;
	mat4 u_projection;
};

out vec3 cubemapTexCoord;

void main()
{
	vec4 pos = u_projection * mat4(mat3(u_view)) * vec4(v_pos, 1.0f);
	gl_Position = pos.xyww;
	cubemapTexCoord = 2 * v_pos;
}