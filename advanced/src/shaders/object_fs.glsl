#version 460 core

struct DirectionalLight
{
	vec3 dirNormalized; // 0
	vec3 diffuseColor; // 4N
	vec3 specularColor; // 8N
};

struct PointLight
{
	vec3 pos; // 0
	vec3 diffuseColor; // 4N 
	vec3 specularColor; // 8N
	float attenConst, attenLin, attenQuad; // 12N, 13N, 14N
};

struct ConeLight
{
	vec3 pos; // 0
	vec3 dirNormalized; // 4N
	vec3 diffuseColor; // 8N
	vec3 specularColor; // 12N
	float angleCosine; // 16N
	float attenConst, attenLin, attenQuad; // 17N, 18N, 19N
};	

in vec2 texCoord;
in mat3 TBNMatrix;
in vec3 fragWorldCoord;
out vec4 fragDirLightSpaceCoords[MAX_DIR_LIGHTS];

uniform sampler2D u_diffuse;
uniform sampler2D u_specular;
uniform sampler2D u_emissive;
uniform sampler2D u_normal;
uniform sampler2D u_displacement;
uniform float u_materialShininess;
uniform float u_heightScale;
uniform int u_numDispLayers;
uniform float u_emissiveStrength;

const int MAX_DIR_LIGHTS = 10;
const int MAX_POINT_LIGHTS = 100;

layout(std140) uniform lights
{
	DirectionalLight u_dirLights[MAX_DIR_LIGHTS];
	PointLight u_pointLights[MAX_POINT_LIGHTS];
};

layout(std140) uniform numDirLights
{
	int u_numDirLights;
};

layout(std140) uniform numPointLights
{
	int u_numPointLights;
};

uniform float u_ambience;
uniform vec3 u_viewPos;

uniform sampler2DArray u_dirLightShadowMaps;
uniform samplerCubeArray u_pointLightShadowMaps;
layout(std140) uniform pointLightFarPlane
{
	float u_pointLightFarPlane;
};

out vec4 fragColor;

vec2 displaceTexCoord(vec2 texCoord)
{
	vec2 fullOffset = (transpose(TBNMatrix) * normalize(u_viewPos - fragWorldCoord)).xy * u_heightScale;

	vec2 deltaOffset = fullOffset / u_numDispLayers;
	float deltaDepth = 1.0f / u_numDispLayers;

	vec2 currentOffset = vec2(0.0f);
	float currentDepth = 0.0f;
	float currentDepthMapValue = texture(u_displacement, texCoord).r;

	while(currentDepth < currentDepthMapValue)
	{
		currentOffset += deltaOffset;
		currentDepth += deltaDepth;
		currentDepthMapValue = texture(u_displacement, texCoord - currentOffset).r;
	}

	/*
	vec2 prevOffset = currentOffset - deltaOffset;
	float prevDepthMapValue = texture(u_displacement, texCoord - prevOffset).r;

	float currentDepthMapDelta = currentDepth - currentDepthMapValue;
	float prevDepthMapDelta = prevDepthMapValue - currentDepth;

	// This will happen when displacement map is flat (all zeroes)
	if((prevDepthMapDelta + currentDepthMapDelta) == 0)
	{
		return texCoord;
	}

	vec2 actualOffset = ((currentDepthMapDelta * prevOffset) + (prevDepthMapDelta * currentOffset)) / 
						(currentDepthMapDelta + prevDepthMapDelta);

	return texCoord - actualOffset;
	*/

	return texCoord - currentOffset;
}

vec2 dispTexCoord = displaceTexCoord(texCoord); 
vec3 normal = normalize(TBNMatrix * ((2.0f * texture(u_normal, dispTexCoord).xyz) - vec3(1.0f)));

vec3 calcDirLight(DirectionalLight light, vec3 diffuseFragColor, vec3 specularFragColor)
{
	float diffuseStrength = clamp(dot(-light.dirNormalized, normal), 0.0f, 1.0f);
	vec3 diffuseColor = light.diffuseColor * diffuseFragColor * diffuseStrength;

	vec3 halfwayVector = normalize(u_viewPos - fragWorldCoord) - light.dirNormalized;
	float specularStrength = pow(clamp(dot(normal, normalize(halfwayVector)), 0.0f, 1.0f), u_materialShininess);
	vec3 specularColor = light.specularColor * specularFragColor * specularStrength;

	return diffuseColor + specularColor;
}

vec3 calcPointLight(PointLight light, vec3 diffuseFragColor, vec3 specularFragColor)
{
	vec3 lightDisp = light.pos - fragWorldCoord;
	float lightDist = length(lightDisp);
	vec3 lightFragDir = normalize(lightDisp);
	float attenRatio = 1.0f / (light.attenConst + (lightDist * light.attenLin) + (lightDist * lightDist * light.attenQuad));

	float diffuseStrength = clamp(dot(lightFragDir, normal), 0.0f, 1.0f) * attenRatio;
	vec3 diffuseColor = light.diffuseColor * diffuseFragColor * diffuseStrength;

	vec3 halfwayVector = normalize(u_viewPos - fragWorldCoord) + lightFragDir;
	float specularStrength = pow(clamp(dot(normal, normalize(halfwayVector)), 0.0f, 1.0f), u_materialShininess) * attenRatio;
	vec3 specularColor = light.specularColor * specularFragColor * specularStrength;

	return diffuseColor + specularColor;
}

vec3 calcConeLight(ConeLight light, vec3 diffuseFragColor, vec3 specularFragColor)
{
	vec3 lightDisp = light.pos - fragWorldCoord;
	vec3 lightFragDir = normalize(lightDisp);

	if(dot(light.dirNormalized, -lightFragDir) < light.angleCosine)
	{
		return vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	float lightDist = length(lightDisp);
	float attenRatio = 1.0f / (light.attenConst + (lightDist * light.attenLin) + (lightDist * lightDist * light.attenQuad));

	float diffuseStrength = clamp(dot(lightFragDir, normal), 0.0f, 1.0f) * attenRatio;
	vec3 diffuseColor = light.diffuseColor * diffuseFragColor * diffuseStrength;

	vec3 halfwayVector = normalize(u_viewPos - fragWorldCoord) + lightFragDir;
	float specularStrength = pow(clamp(dot(normal, normalize(halfwayVector)), 0.0f, 1.0f), u_materialShininess) * attenRatio;
	vec3 specularColor = light.specularColor * specularFragColor * specularStrength;

	return diffuseColor + specularColor;
}

float shadowSampleOffsetValue = 1 / 1000.0f;

vec2 shadowSampleOffsets[9] =
{
	vec2(-shadowSampleOffsetValue, shadowSampleOffsetValue), 
	vec2(0, shadowSampleOffsetValue), 
	vec2(shadowSampleOffsetValue, shadowSampleOffsetValue), 
	vec2(-shadowSampleOffsetValue, 0), 
	vec2(0, 0), 
	vec2(shadowSampleOffsetValue, 0),
	vec2(-shadowSampleOffsetValue, -shadowSampleOffsetValue), 
	vec2(0, -shadowSampleOffsetValue), 
	vec2(shadowSampleOffsetValue, -shadowSampleOffsetValue)
};

void main()
{
	vec4 diffuseColor = texture(u_diffuse, dispTexCoord);
	vec4 specularColor = texture(u_specular, dispTexCoord);
	vec4 emissiveColor = texture(u_emissive, dispTexCoord);

	fragColor.rgb = vec3(0.0f, 0.0f, 0.0f);
	fragColor.rgb += u_ambience * diffuseColor.rgb;
	fragColor.rgb += emissiveColor.rgb * u_emissiveStrength;
	fragColor.a = diffuseColor.a;

	for(int dirLightIdx = 0; dirLightIdx < u_numDirLights; dirLightIdx++)
	{
		float dirLightShadowStrength = 0.0f;

		for(int i = 0; i < 9; i++)
		{
			float depthBias = 0.001f * (1.0f - dot(normal, -u_dirLights[dirLightIdx].dirNormalized));
			vec2 shadowSampleTexCoord = (fragDirLightSpaceCoord.xy * 0.5f) + vec2(0.5f) + shadowSampleOffsets[i];

			float closestLightDepth = texture(u_dirLightShadowMaps[dirLightIdx], shadowSampleTexCoord).r;
			float fragLightDepth = ((fragDirLightSpaceCoord.z * 0.5f) + 0.5f) - depthBias;

			if(fragLightDepth > closestLightDepth)
			{
				dirLightShadowStrength += 1.0f / 9.0f;
			}
		}

		fragColor.rgb += (1.0f - dirLightShadowStrength) * calcDirLight(u_dirLight[dirLightIdx], diffuseColor.rgb, specularColor.rgb);
	}

	for(int pointLightIdx = 0; pointLightIdx < u_numPointLights; pointLightIdx++)
	{
		vec3 shadowSampleTexCoord = fragWorldCoord - u_pointLights[pointLightIdx].pos;
		float depthBias = 0.001f * (1.0f - dot(normal, normalize(-shadowSampleTexCoord)));
		
		float pointLightShadowStrength = 0.0f;
		float closestLightDepth = texture(u_pointLightShadowMaps[pointLightIdx], shadowSampleTexCoord).r;
		float fragLightDepth = (length(shadowSampleTexCoord) / u_pointLightFarPlane) - depthBias;

		if(fragLightDepth > closestLightDepth)
		{
			pointLightShadowStrength = 1.0f;
		}

		fragColor.rgb += (1.0f - pointLightShadowStrength) * 
						 calcPointLight(u_pointLights[pointLightIdx], diffuseColor.rgb, specularColor.rgb);
	}
}