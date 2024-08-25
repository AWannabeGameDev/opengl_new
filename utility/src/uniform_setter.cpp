#include <utility/uniform_setter.h>
#include <utility/util.h>
#include <glm/gtc/type_ptr.hpp>

UniformSetter::UniformSetter() :
	names{2}
{}

void UniformSetter::addUniform(unsigned int shaderID, std::string_view fieldName)
{
	auto it = names.addInPlace(fieldName);
	std::string_view fieldNameStrView{*it};
	nameToLocation[std::make_pair(shaderID, fieldNameStrView)] = glGetUniformLocation(shaderID, fieldName.data());
}

void UniformSetter::setUniform(unsigned int shaderID, std::string_view fieldName, const glm::vec3& vec3)
{
		glUniform3fv(nameToLocation.at(std::make_pair(shaderID, fieldName)), 1, &vec3[0]);
}

void UniformSetter::setUniform(unsigned int shaderID, std::string_view fieldName, const glm::vec4& vec4)
{
	glUniform4fv(nameToLocation.at(std::make_pair(shaderID, fieldName)), 1, &vec4[0]);
}

void UniformSetter::setUniform(unsigned int shaderID, std::string_view fieldName, float flt)
{
	glUniform1f(nameToLocation.at(std::make_pair(shaderID, fieldName)), flt);
}

void UniformSetter::setUniform(unsigned int shaderID, std::string_view fieldName, const glm::mat4& mat4)
{
	glUniformMatrix4fv(nameToLocation.at(std::make_pair(shaderID, fieldName)), 1, GL_FALSE, glm::value_ptr(mat4));
}

void UniformSetter::setUniform(unsigned int shaderID, std::string_view fieldName, const glm::mat3& mat3)
{
	glUniformMatrix3fv(nameToLocation.at(std::make_pair(shaderID, fieldName)), 1, GL_FALSE, glm::value_ptr(mat3));
}

void UniformSetter::setUniform(unsigned int shaderID, std::string_view fieldName, int integ)
{
	glUniform1i(nameToLocation.at(std::make_pair(shaderID, fieldName)), integ);
}