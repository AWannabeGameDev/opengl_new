#ifndef UNIFORM_SETTER_H
#define UNIFORM_SETTER_H

#include <unordered_map>
#include <string_view>
#include <utility/unrolled_list.h>
#include <string>
#include <glm/glm.hpp>

template<typename T1, typename T2>
struct std::hash<std::pair<T1, T2>>
{
	std::size_t operator()(const std::pair<T1, T2>& key) const
	{
		std::hash<T1> hash1;
		std::hash<T2> hash2;

		std::size_t seed = hash1(key.first);
		seed ^= hash2(key.second) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

		return seed;
	}
};

class UniformSetter
{
private :
	UnrolledList<std::string> names;
	std::unordered_map<std::pair<unsigned int, std::string_view>, int> nameToLocation;

	int getUniformLocation(unsigned int shaderID, std::string_view fieldName);

public :
	UniformSetter();

	void addUniform(unsigned int shaderID, std::string_view fieldName);

	void setUniform(unsigned int shaderID, std::string_view fieldName, const glm::vec3& vec3);
	void setUniform(unsigned int shaderID, std::string_view fieldName, const glm::vec4& vec4);
	void setUniform(unsigned int shaderID, std::string_view fieldName, float flt);
	void setUniform(unsigned int shaderID, std::string_view fieldName, const glm::mat4& mat4);
	void setUniform(unsigned int shaderID, std::string_view fieldName, const glm::mat3& mat3);
	void setUniform(unsigned int shaderID, std::string_view fieldName, int integ);

	void bindUniformBlock(unsigned int shaderID, std::string_view blockName, unsigned int bindingIndex);
};

#endif