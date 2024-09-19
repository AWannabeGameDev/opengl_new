#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

struct PointLight
{
	glm::vec3 position;
	float _padding1;
	glm::vec3 diffuseColor;
	float _padding2;
	glm::vec3 specularColor;
	float attenConst;
	float attenLin;
	float attenQuad;
	float _padding4[2];
};

struct DirectionalLight
{
	glm::vec3 direction;
	float _padding1;
	glm::vec3 diffuseColor;
	float _padding2;
	glm::vec3 specularColor;
	float _padding3;
};

struct ConeLight
{
	glm::vec3 position;
	float _padding1;
	glm::vec3 direction;
	float _padding2;
	glm::vec3 diffuseColor;
	float _padding3;
	glm::vec3 specularColor;
	float angleCosine;
	float attenConst;
	float attenLin;
	float attenQuad;
	float _padding4;
};

#endif