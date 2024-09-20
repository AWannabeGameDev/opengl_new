#include <utility/uniform_setter.h>
#include <utility/util.h>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <format>

UniformSetter::UniformSetter() :
	names{2}
{}

void UniformSetter::addUniform(unsigned int shaderID, std::string_view fieldName)
{
	auto it = names.addInPlace(fieldName);
	std::string_view fieldNameStrView{*it};
	int location = glGetUniformLocation(shaderID, fieldName.data());

	if(location != -1)
	{
		nameToLocation[std::make_pair(shaderID, fieldNameStrView)] = location;
	}
	else
	{
		throw std::invalid_argument{std::format("Field {} does not exist in shader {}", fieldName, shaderID)};
	}
}

int UniformSetter::getUniformLocation(unsigned int shaderID, std::string_view fieldName)
{
	try
	{
		return nameToLocation.at(std::make_pair(shaderID, fieldName));
	}
	catch(const std::out_of_range& exp)
	{
		throw std::invalid_argument{std::format("Combination of shaderID {} and field {} does not exist.", shaderID, fieldName)};
	}
}

void UniformSetter::setUniform(unsigned int shaderID, std::string_view fieldName, const glm::vec3& vec3)
{
	glUniform3fv(getUniformLocation(shaderID, fieldName), 1, &vec3[0]);
}

void UniformSetter::setUniform(unsigned int shaderID, std::string_view fieldName, const glm::vec4& vec4)
{
	glUniform4fv(getUniformLocation(shaderID, fieldName), 1, &vec4[0]);
}

void UniformSetter::setUniform(unsigned int shaderID, std::string_view fieldName, float flt)
{
	glUniform1f(getUniformLocation(shaderID, fieldName), flt);
}

void UniformSetter::setUniform(unsigned int shaderID, std::string_view fieldName, const glm::mat4& mat4)
{
	glUniformMatrix4fv(getUniformLocation(shaderID, fieldName), 1, GL_FALSE, glm::value_ptr(mat4));
}

void UniformSetter::setUniform(unsigned int shaderID, std::string_view fieldName, const glm::mat3& mat3)
{
	glUniformMatrix3fv(getUniformLocation(shaderID, fieldName), 1, GL_FALSE, glm::value_ptr(mat3));
}

void UniformSetter::setUniform(unsigned int shaderID, std::string_view fieldName, int integ)
{
	glUniform1i(getUniformLocation(shaderID, fieldName), integ);
}

void UniformSetter::bindUniformBlock(unsigned int shaderID, std::string_view blockName, unsigned int bindingIndex)
{
	unsigned int blockIndex = glGetUniformBlockIndex(shaderID, blockName.data());

	if(blockIndex == GL_INVALID_INDEX)
	{
		throw std::invalid_argument{std::format("{} is not a valid uniform block name in shader {}", blockName, shaderID)};
	}

	glUniformBlockBinding(shaderID, blockIndex, bindingIndex);
}