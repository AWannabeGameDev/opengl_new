#version 460 core

in vec2 texCoord;

uniform sampler2D fbTexture;

out vec4 fragColor;

const float offset = 1.0f / 300.0f;

const vec2 offsets[9] =
{
	vec2(-offset, offset),  vec2(0.0f, offset),  vec2(offset, offset),
	vec2(-offset, 0.0f),    vec2(0.0f, 0.0f),    vec2(offset, 0.0f),
	vec2(-offset, -offset), vec2(0.0f, -offset), vec2(offset, -offset)
};

const float redKernel[9] =
{
	0.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 0.0f
};

const float blueKernel[9] =
{
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f
};

void main()
{
	fragColor.rb = vec2(0.0f, 0.0f);
	fragColor.ga = texture(fbTexture, texCoord).ga;

	for(int i = 0; i < 9; i++)
	{
		fragColor.r += texture(fbTexture, texCoord + offsets[i]).r * redKernel[i];
		fragColor.b += texture(fbTexture, texCoord + offsets[i]).b * blueKernel[i];
	}
	fragColor.r /= 3.0f;
	fragColor.b /= 3.0f;

	float gamma = 2.2f;
	fragColor.rgb = pow(fragColor.rgb, vec3(1.0f / gamma));
}