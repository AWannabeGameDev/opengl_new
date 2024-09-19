#version 460 core

const int MAX_POINT_LIGHTS = 100;

layout(triangles) in;
layout(triangle_strip, max_vertices = 18 * MAX_POINT_LIGHTS) out;

layout(std140) uniform pointLightMatrices
{
	mat4 u_lightSpacePositionMatrices[MAX_POINT_LIGHTS];
	mat4 u_lightSpaceProjMatrix;
	mat4 u_lightSpaceViewMatrices[6];
};

layout(std140) uniform numPointLights
{
	int u_numPointLights;
};

out vec3 fragViewCoord;

void main()
{
	for(int pointLightIdx = 0; pointLightIdx < u_numPointLights; pointLightIdx++)
	{
		for(int face = 0; face < 6; face++)
		{
			gl_Layer = (6 * pointLightIdx) + face;

			for(int i = 0; i < 3; i++)
			{
				fragViewCoord = (u_lightSpaceViewMatrices[face] * 
								 u_lightSpacePositionMatrices[pointLightIdx] * gl_in[i].gl_Position).xyz;
				gl_Position = u_lightSpaceProjMatrix * vec4(fragViewCoord, 1.0f);
				EmitVertex();
			}

			EndPrimitive();
		}
	}
}