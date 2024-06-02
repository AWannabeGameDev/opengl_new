#ifndef STRUCT_UNIFORM_UTIL_H
#define STRUCT_UNIFORM_UTIL_H

#include <utility/uniform_setter.h>
#include <utility/lighting.h>

void addDirLightUniforms(UniformSetter& uniforms, unsigned int shaderID, std::string_view name);

void setDirLightUniform(UniformSetter& uniforms, unsigned int shaderID, std::string_view name,
						const DirectionalLight& light);

void addPointLightUniforms(UniformSetter& uniforms, unsigned int shaderID, std::string_view name);

void setPointLightUniform(UniformSetter& uniforms, unsigned int shaderID, std::string_view name, const PointLight& light);

#endif