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

struct PointLightRender
{
	unsigned int shadowCubeMap;
	glm::mat4 projMatrix;
	glm::mat4 viewMatrices[6];
	glm::mat4 positionMatrix;
	float farPlane;
	PointLight source;
};

struct DirectionalLightRender
{
	unsigned int shadowMap;
	glm::mat4 matrix;
	DirectionalLight source;
};

#endif