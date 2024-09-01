#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

struct PointLight
{
	glm::vec3 position;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
	float attenConst;
	float attenLin;
	float attenQuad;
};

struct DirectionalLight
{
	glm::vec3 direction;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
};

struct ConeLight
{
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
	float angleCosine;
	float attenConst;
	float attenLin;
	float attenQuad;
};

#endif