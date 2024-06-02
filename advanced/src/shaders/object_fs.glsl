#version 460 core

struct DirectionalLight
{
	vec3 dirNormalized;
	vec3 diffuseColor; 
	vec3 specularColor;
};

struct PointLight
{
	vec3 pos; 
	vec3 diffuseColor; 
	vec3 specularColor;
	float attenConst, attenLin, attenQuad; 
};

struct ConeLight
{
	vec3 pos;
	vec3 dirNormalized; 
	vec3 diffuseColor; 
	vec3 specularColor;
	float angleCosine; 
	float attenConst, attenLin, attenQuad; 
};	

in vec2 texCoord;
in vec3 normal;
in vec3 fragWorldCoord;
in vec4 fragDirLightSpaceCoord;

uniform sampler2D u_diffuse;
uniform sampler2D u_specular;
uniform sampler2D u_emissive;
uniform float u_materialShininess;

uniform DirectionalLight u_dirLight;
uniform PointLight u_pointLight;
uniform float u_ambience;
uniform vec3 u_viewPos;

uniform sampler2D u_dirLightShadowMap;
uniform samplerCube u_pointLightShadowMap;
uniform float u_pointLightFarPlane;

out vec4 fragColor;

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
	vec4 diffuseColor = texture(u_diffuse, texCoord);
	vec4 specularColor = texture(u_specular, texCoord);
	vec4 emissiveColor = texture(u_emissive, texCoord);

	fragColor.rgb = vec3(0.0f, 0.0f, 0.0f);
	fragColor.rgb += u_ambience * diffuseColor.rgb;
	fragColor.rgb += emissiveColor.rgb;
	fragColor.a = diffuseColor.a;

	float dirLightShadowStrength = 0.0f;

	for(int i = 0; i < 9; i++)
	{
		float closestLightDepth = texture(u_dirLightShadowMap, 
										  (fragDirLightSpaceCoord.xy * 0.5f) + vec2(0.5f) + shadowSampleOffsets[i]).r;
		float fragLightDepth = ((fragDirLightSpaceCoord.z * 0.5f) + 0.5f) - 
								(0.001f * (1.0f - dot(normal, -u_dirLight.dirNormalized)));

		if(fragLightDepth > closestLightDepth)
		{
			dirLightShadowStrength += 1.0f / 9.0f;
		}
	}

	fragColor.rgb += (1.0f - dirLightShadowStrength) * calcDirLight(u_dirLight, diffuseColor.rgb, specularColor.rgb);

	float pointLightShadowStrength = 0.0f;

	for(int i = 0; i < 9; i++)
	{
		float closestLightDepth = texture(u_pointLightShadowMap, 
										  fragWorldCoord - u_pointLight.pos).r;
		float fragLightDepth = (length(fragWorldCoord - u_pointLight.pos) / u_pointLightFarPlane) + 
								(0.001f * (1.0f - dot(normal, u_pointLight.pos - fragWorldCoord)));

		if(fragLightDepth > closestLightDepth)
		{
			pointLightShadowStrength += 1.0f / 9.0f;
		}
	}

	fragColor.rgb += (1.0f - pointLightShadowStrength) * calcPointLight(u_pointLight, diffuseColor.rgb, specularColor.rgb);
}