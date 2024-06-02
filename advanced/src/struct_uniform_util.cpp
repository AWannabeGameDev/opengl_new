#include "struct_uniform_util.h"

void addDirLightUniforms(UniformSetter& uniforms, unsigned int shaderID, std::string_view name)
{
	std::string nameStr{name};

	std::string fieldNameStr{nameStr + ".dirNormalized"};
	uniforms.addUniform(shaderID, fieldNameStr);

	fieldNameStr = nameStr + ".diffuseColor";
	uniforms.addUniform(shaderID, fieldNameStr);

	fieldNameStr = nameStr + ".specularColor";
	uniforms.addUniform(shaderID, fieldNameStr);
}

void setDirLightUniform(UniformSetter& uniforms, unsigned int shaderID, std::string_view name,
						const DirectionalLight& light)
{
	std::string nameStr{name};

	std::string fieldNameStr{nameStr + ".dirNormalized"};
	uniforms.setUniform(shaderID, fieldNameStr, light.direction);

	fieldNameStr = nameStr + ".diffuseColor";
	uniforms.setUniform(shaderID, fieldNameStr, light.diffuseColor);

	fieldNameStr = nameStr + ".specularColor";
	uniforms.setUniform(shaderID, fieldNameStr, light.specularColor);
}

void addPointLightUniforms(UniformSetter& uniforms, unsigned int shaderID, std::string_view name)
{
	std::string nameStr{name};

	std::string fieldNameStr{nameStr + ".pos"};
	uniforms.addUniform(shaderID, fieldNameStr);

	fieldNameStr = nameStr + ".diffuseColor";
	uniforms.addUniform(shaderID, fieldNameStr);

	fieldNameStr = nameStr + ".specularColor";
	uniforms.addUniform(shaderID, fieldNameStr);

	fieldNameStr = nameStr + ".attenConst";
	uniforms.addUniform(shaderID, fieldNameStr);

	fieldNameStr = nameStr + ".attenLin";
	uniforms.addUniform(shaderID, fieldNameStr);

	fieldNameStr = nameStr + ".attenQuad";
	uniforms.addUniform(shaderID, fieldNameStr);
}

void setPointLightUniform(UniformSetter& uniforms, unsigned int shaderID, std::string_view name, const PointLight& light)
{
	std::string nameStr{name};

	std::string fieldNameStr{nameStr + ".pos"};
	uniforms.setUniform(shaderID, fieldNameStr, light.position);

	fieldNameStr = nameStr + ".diffuseColor";
	uniforms.setUniform(shaderID, fieldNameStr, light.diffuseColor);

	fieldNameStr = nameStr + ".specularColor";
	uniforms.setUniform(shaderID, fieldNameStr, light.specularColor);

	fieldNameStr = nameStr + ".attenConst";
	uniforms.setUniform(shaderID, fieldNameStr, light.attenConst);

	fieldNameStr = nameStr + ".attenLin";
	uniforms.setUniform(shaderID, fieldNameStr, light.attenLin);

	fieldNameStr = nameStr + ".attenQuad";
	uniforms.setUniform(shaderID, fieldNameStr, light.attenQuad);
}