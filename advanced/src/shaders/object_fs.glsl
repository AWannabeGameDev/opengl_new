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

in vsLightOut
{
	DirectionalLight TBNdirLight;
	PointLight TBNpointLight;
	vec3 TBNviewPos;
	vec3 TBNfragWorldCoord;
}
lightIn;

in vsShadowOut
{
	vec4 fragDirLightSpaceCoord;
	vec3 fragPointLightSpaceCoord;
}
shadowIn;

in vec2 texCoord;

uniform sampler2D u_diffuse;
uniform sampler2D u_specular;
uniform sampler2D u_emissive;
uniform sampler2D u_normal;
uniform float u_materialShininess;

uniform float u_ambience;

uniform sampler2D u_dirLightShadowMap;
uniform samplerCube u_pointLightShadowMap;
uniform float u_pointLightFarPlane;

out vec4 fragColor;

vec3 normal = normalize((2.0f * texture(u_normal, texCoord).xyz) - vec3(1.0f));

vec3 calcDirLight(DirectionalLight light, vec3 diffuseFragColor, vec3 specularFragColor)
{
	float diffuseStrength = clamp(dot(-light.dirNormalized, normal), 0.0f, 1.0f);
	vec3 diffuseColor = light.diffuseColor * diffuseFragColor * diffuseStrength;

	vec3 halfwayVector = normalize(lightIn.TBNviewPos - lightIn.TBNfragWorldCoord) - light.dirNormalized;
	float specularStrength = pow(clamp(dot(normal, normalize(halfwayVector)), 0.0f, 1.0f), u_materialShininess);
	vec3 specularColor = light.specularColor * specularFragColor * specularStrength;

	return diffuseColor + specularColor;
}

vec3 calcPointLight(PointLight light, vec3 diffuseFragColor, vec3 specularFragColor)
{
	vec3 lightDisp = light.pos - lightIn.TBNfragWorldCoord;
	float lightDist = length(lightDisp);
	vec3 lightFragDir = normalize(lightDisp);
	float attenRatio = 1.0f / (light.attenConst + (lightDist * light.attenLin) + (lightDist * lightDist * light.attenQuad));

	float diffuseStrength = clamp(dot(lightFragDir, normal), 0.0f, 1.0f) * attenRatio;
	vec3 diffuseColor = light.diffuseColor * diffuseFragColor * diffuseStrength;

	vec3 halfwayVector = normalize(lightIn.TBNviewPos - lightIn.TBNfragWorldCoord) + lightFragDir;
	float specularStrength = pow(clamp(dot(normal, normalize(halfwayVector)), 0.0f, 1.0f), u_materialShininess) * attenRatio;
	vec3 specularColor = light.specularColor * specularFragColor * specularStrength;

	return diffuseColor + specularColor;
}

vec3 calcConeLight(ConeLight light, vec3 diffuseFragColor, vec3 specularFragColor)
{
	vec3 lightDisp = light.pos - lightIn.TBNfragWorldCoord;
	vec3 lightFragDir = normalize(lightDisp);

	if(dot(light.dirNormalized, -lightFragDir) < light.angleCosine)
	{
		return vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	float lightDist = length(lightDisp);
	float attenRatio = 1.0f / (light.attenConst + (lightDist * light.attenLin) + (lightDist * lightDist * light.attenQuad));

	float diffuseStrength = clamp(dot(lightFragDir, normal), 0.0f, 1.0f) * attenRatio;
	vec3 diffuseColor = light.diffuseColor * diffuseFragColor * diffuseStrength;

	vec3 halfwayVector = normalize(lightIn.TBNviewPos - lightIn.TBNfragWorldCoord) + lightFragDir;
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
		float depthBias = 0.001f * (1.0f - dot(normal, -lightIn.TBNdirLight.dirNormalized));
		vec2 shadowSampleTexCoord = (shadowIn.fragDirLightSpaceCoord.xy * 0.5f) + vec2(0.5f) + shadowSampleOffsets[i];

		float closestLightDepth = texture(u_dirLightShadowMap, shadowSampleTexCoord).r;
		float fragLightDepth = ((shadowIn.fragDirLightSpaceCoord.z * 0.5f) + 0.5f) - depthBias;

		if(fragLightDepth > closestLightDepth)
		{
			dirLightShadowStrength += 1.0f / 9.0f;
		}
	}

	fragColor.rgb += (1.0f - dirLightShadowStrength) * calcDirLight(lightIn.TBNdirLight, diffuseColor.rgb, specularColor.rgb);

	float depthBias = 0.001f * (1.0f - dot(normal, normalize(lightIn.TBNpointLight.pos - lightIn.TBNfragWorldCoord)));
	
	float pointLightShadowStrength = 0.0f;
	float closestLightDepth = texture(u_pointLightShadowMap, shadowIn.fragPointLightSpaceCoord).r;
	float fragLightDepth = (length(shadowIn.fragPointLightSpaceCoord) / u_pointLightFarPlane) - depthBias;

	if(fragLightDepth > closestLightDepth)
	{
		pointLightShadowStrength = 1.0f;
	}

	fragColor.rgb += (1.0f - pointLightShadowStrength) * calcPointLight(lightIn.TBNpointLight, diffuseColor.rgb, specularColor.rgb);
}