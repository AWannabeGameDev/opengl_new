#version 460 core

const int MAX_DIR_LIGHTS = 3;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3 * MAX_DIR_LIGHTS) out;

layout(std140) uniform dirLightMatrices
{
	mat4 u_lightSpaceMatrices[MAX_DIR_LIGHTS];
};

layout(std140) uniform numDirLights
{
	int u_numDirLights;
};

void main()
{
    for(int dirLightIdx = 0; dirLightIdx < u_numDirLights; dirLightIdx++)
    {
        gl_Layer = dirLightIdx;

        for(int i = 0; i < 3; i++)
        {
            gl_Position = u_lightSpaceMatrices[dirLightIdx] * gl_in[i].gl_Position;
            EmitVertex();
        }

        EndPrimitive();
    }
}