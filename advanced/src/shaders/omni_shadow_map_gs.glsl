#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 u_lightSpaceProjMatrix;
uniform mat4 u_lightSpaceViewMatrices[6];

out vec3 fragViewCoord;

void main()
{
	for(int face = 0; face < 6; face++)
	{
		gl_Layer = face;

		for(int i = 0; i < 3; i++)
		{
			fragViewCoord = (u_lightSpaceViewMatrices[face] * gl_in[i].gl_Position).xyz;
			gl_Position = u_lightSpaceProjMatrix * vec4(fragViewCoord, 1.0f);
			EmitVertex();
		}

		EndPrimitive();
	}
}