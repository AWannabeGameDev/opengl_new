#include "application.h"
#include <gl_util/raw_model_data.h>
#include <ctime>
#include <gl_util/texture.h>
#include <format>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "struct_uniform_util.h"
#include "constants.h"

using namespace models;

void Application::loadTextures()
{
	cubeShininess = 128.0f; 
	floorShininess = 32.0f;
	lightCubeEmissiveStrength = 1.0f;
	floorHeightScale = 0.07f;
	numDispLayers = 10;

	TextureParameterSet texParams;
	texParams.minFilter = GL_LINEAR_MIPMAP_LINEAR;
	texParams.magFilter = GL_LINEAR;
	texParams.texWrapS = GL_REPEAT;
	texParams.texWrapT = GL_REPEAT;
	texParams.texWrapR = GL_REPEAT;

	woodCubeModelInfo.diffuseMapID = createTexture(GL_TEXTURE_2D, texParams, GL_SRGB, "../res/container_diffuse.png", true);
	woodCubeModelInfo.specularMapID = createTexture(GL_TEXTURE_2D, texParams, GL_RGB, "../res/container_specular.png", true);

	floorModelInfo.diffuseMapID = createTexture(GL_TEXTURE_2D, texParams, GL_SRGB, "../res/bricks2_diffuse.jpg", false);
	//floorSpecularTexture = createTexture(GL_TEXTURE_2D, texParams, GL_RGB, "../res/brick_specular.png", true);
	floorModelInfo.normalMapID = createTexture(GL_TEXTURE_2D, texParams, GL_RGB, "../res/bricks2_normal.jpg", false);
	floorModelInfo.dispMapID = createTexture(GL_TEXTURE_2D, texParams, GL_RGB, "../res/bricks2_displacement.jpg", false);

	texParams.minFilter = GL_LINEAR;
	texParams.magFilter = GL_LINEAR;
	texParams.texWrapS = GL_CLAMP_TO_EDGE;
	texParams.texWrapT = GL_CLAMP_TO_EDGE;
	texParams.texWrapR = GL_CLAMP_TO_EDGE;

	std::string_view skyboxPaths[6]
	{
		"../res/skybox_pos_x.png", "../res/skybox_neg_x.png",
		"../res/skybox_pos_y.png", "../res/skybox_neg_y.png",
		"../res/skybox_pos_z.png", "../res/skybox_neg_z.png"
	};

	skybox = createCubemap(texParams, GL_SRGB, skyboxPaths, false);
	glActiveTexture(GL_TEXTURE0 + SKYBOX_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);

	texParams.minFilter = GL_NEAREST;
	texParams.magFilter = GL_NEAREST;
	texParams.texWrapS = GL_REPEAT;
	texParams.texWrapT = GL_REPEAT;
	texParams.texWrapR = GL_REPEAT;

	blackTexture = createTexture(GL_TEXTURE_2D, texParams, GL_RGB16F, 1, 1);
	whiteTexture = createTexture(GL_TEXTURE_2D, texParams, GL_RGB16F, 1, 1);
	lightCubeModelInfo.emissiveMapID = createTexture(GL_TEXTURE_2D, texParams, GL_RGB16F, 1, 1);
	defaultNormalTexture = createTexture(GL_TEXTURE_2D, texParams, GL_RGB16F, 1, 1);

	glGenFramebuffers(1, &textureWriteFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, textureWriteFBO);
	glViewport(0, 0, 1, 1);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightCubeModelInfo.emissiveMapID, 0);
	glClearColor(0.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blackTexture, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, whiteTexture, 0);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, defaultNormalTexture, 0);
	glClearColor(0.5f, 0.5f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	woodCubeModelInfo.emissiveMapID = blackTexture;
	woodCubeModelInfo.normalMapID = defaultNormalTexture;
	woodCubeModelInfo.dispMapID = blackTexture;

	lightCubeModelInfo.diffuseMapID = blackTexture;
	lightCubeModelInfo.specularMapID = blackTexture;
	lightCubeModelInfo.normalMapID = defaultNormalTexture;
	lightCubeModelInfo.dispMapID = blackTexture;

	floorModelInfo.specularMapID = whiteTexture;
	floorModelInfo.emissiveMapID = blackTexture;

	uniforms.addUniform(shader, "u_diffuse");
	uniforms.addUniform(shader, "u_specular");
	uniforms.addUniform(shader, "u_emissive");
	uniforms.addUniform(shader, "u_normal");
	uniforms.addUniform(shader, "u_displacement");
	uniforms.addUniform(shader, "u_materialShininess");
	uniforms.addUniform(shader, "u_heightScale");
	uniforms.addUniform(shader, "u_numDispLayers");
	uniforms.addUniform(shader, "u_emissiveStrength");

	glUseProgram(shader);
	uniforms.setUniform(shader, "u_diffuse", DIFFUSE_TEXTURE_UNIT);   // These five can
	uniforms.setUniform(shader, "u_specular", SPECULAR_TEXTURE_UNIT); // have texture-arrays
	uniforms.setUniform(shader, "u_emissive", EMISSIVE_TEXTURE_UNIT); // of the same size
	uniforms.setUniform(shader, "u_normal", NORMAL_TEXTURE_UNIT);
	uniforms.setUniform(shader, "u_displacement", DISPLACEMENT_TEXTURE_UNIT);

	uniforms.addUniform(skyboxShader, "u_skybox");

	glUseProgram(skyboxShader);
	uniforms.setUniform(skyboxShader, "u_skybox", SKYBOX_TEXTURE_UNIT);
}

size_t Application::loadModels()
{
	woodCubeModelInfo.indexCount = cube::NUM_INDICES;
	woodCubeModelInfo.eboOffset = 0;
	woodCubeModelInfo.vboOffset = 0;
	woodCubeModelInfo.instanceCount = NUM_CUBES;
	woodCubeModelInfo.instanceOffset = 0;

	lightCubeModelInfo.indexCount = cube::NUM_INDICES;
	lightCubeModelInfo.eboOffset = 0;
	lightCubeModelInfo.vboOffset = 0;
	lightCubeModelInfo.instanceCount = 1;
	lightCubeModelInfo.instanceOffset = NUM_CUBES;

	floorModelInfo.indexCount = xysquare::NUM_INDICES;
	floorModelInfo.eboOffset = cube::NUM_INDICES;
	floorModelInfo.vboOffset = cube::NUM_VERTS;
	floorModelInfo.instanceCount = 1;
	floorModelInfo.instanceOffset = NUM_CUBES + 1;

	glm::vec2 cubeTexCoords[cube::NUM_VERTS] =
	{
		{1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f},
		{1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f},
		{1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f},
		{1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f},
		{1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f},
		{1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f}
	};

	glm::vec3 cubeTangets[cube::NUM_VERTS] =
	{
		{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
		{-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},
		{-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}
	};

	glm::vec2 floorTexCoords[xysquare::NUM_VERTS] =
	{
		{2.5f, 2.5f}, {0.0f, 2.5f}, {0.0f, 0.0f}, {2.5f, 0.0f}
	};

	glm::vec3 floorTangents[xysquare::NUM_VERTS] =
	{
		{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}
	};

	using std::byte;
	
	std::vector<byte> vboVector;
	vboVector.insert(vboVector.end(), (byte*)cube::positions, (byte*)cube::positions + sizeof(cube::positions));
	vboVector.insert(vboVector.end(), (byte*)xysquare::positions, (byte*)xysquare::positions + sizeof(xysquare::positions));
	vboVector.insert(vboVector.end(), (byte*)cubeTexCoords, (byte*)cubeTexCoords + sizeof(cubeTexCoords));
	vboVector.insert(vboVector.end(), (byte*)floorTexCoords, (byte*)floorTexCoords + sizeof(floorTexCoords));
	vboVector.insert(vboVector.end(), (byte*)cube::normals, (byte*)cube::normals + sizeof(cube::normals));
	vboVector.insert(vboVector.end(), (byte*)xysquare::normals, (byte*)xysquare::normals + sizeof(xysquare::normals));
	vboVector.insert(vboVector.end(), (byte*)cubeTangets, (byte*)cubeTangets + sizeof(cubeTangets));
	vboVector.insert(vboVector.end(), (byte*)floorTangents, (byte*)floorTangents + sizeof(floorTangents));

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vboVector.size(), vboVector.data(), GL_STATIC_DRAW);

	std::vector<byte> eboVector;
	eboVector.insert(eboVector.end(), (byte*)cube::indices, (byte*)cube::indices + sizeof(cube::indices));
	eboVector.insert(eboVector.end(), (byte*)xysquare::indices, (byte*)xysquare::indices + sizeof(xysquare::indices));

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboVector.size(), eboVector.data(), GL_STATIC_DRAW);

	return vboVector.size();
}

void Application::createObjects()
{
	// Later work here : multiple light cubes

	floorWidth = 20.0f;

	srand((unsigned int)time(0));
	for(glm::mat4& cubeTransformMat : cubeTransformMats)
	{
		cubeTransformMat = glm::translate(glm::mat4{1.0f}, glm::vec3{randrange(-9.5f, 9.5f), 0.50f, randrange(-9.5f, 9.5f)});
	}
	
	lightCubeTransformMat = glm::translate(glm::mat4{1.0f}, glm::vec3{0.0f, 0.5f, 0.0f}) * 
							glm::scale(glm::mat4{1.0f}, glm::vec3{0.25f, 0.25f, 0.25f});

	floorTransformMat = glm::rotate(glm::mat4{1.0f}, glm::radians(-90.0f), glm::vec3{1.0f, 0.0f, 0.0f}) * 
						glm::scale(glm::mat4{1.0f}, glm::vec3{floorWidth, floorWidth, 1.0f});

	// Later work here : add texture maps as part of object info

	using std::byte;

	std::vector<byte> instanceVBOVector;
	instanceVBOVector.insert(instanceVBOVector.end(), (byte*)cubeTransformMats,
							 (byte*)cubeTransformMats + sizeof(cubeTransformMats));
	instanceVBOVector.insert(instanceVBOVector.end(), (byte*)&lightCubeTransformMat,
							 (byte*)&lightCubeTransformMat + sizeof(lightCubeTransformMat));
	instanceVBOVector.insert(instanceVBOVector.end(), (byte*)&floorTransformMat,
							 (byte*)&floorTransformMat + sizeof(floorTransformMat));

	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, instanceVBOVector.size(), instanceVBOVector.data(), GL_STATIC_DRAW);
}

void Application::createLightSources()
{
	ambience = 0.1f;

	DirectionalLight& dirLight = dirLights.emplace_back();
	glm::mat4& dirLightMatrix = dirLightMatrices.emplace_back();

	dirLight.direction = glm::normalize(glm::vec3{1.0f, -1.0f, 1.0f});
	dirLight.diffuseColor = glm::vec3{0.6f, 0.6f, 0.6f};
	dirLight.specularColor = glm::vec3{0.3f, 0.3f, 0.3f};

	dirLightMatrix = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, -15.0f, 15.0f) *
		glm::lookAt(-dirLight.direction, glm::vec3{0.0f}, glm::vec3{0.0f, 1.0f, 0.0f});

	maxBrightDiffuseColor = {0.0f, 1.0f, 1.0f};
	maxBrightSpecularColor = {0.0f, 0.4f, 0.4f};

	pointLightFarPlane = 20.0f;
	pointLightProjMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, pointLightFarPlane);

	pointLightViewMatrices[0] = glm::lookAt(glm::vec3{0.0f, 0.0f, 0.0f}, 
											glm::vec3{1.0f, 0.0f, 0.0f}, 
										   glm::vec3{0.0f, -1.0f, 0.0f});

	pointLightViewMatrices[1] = glm::lookAt(glm::vec3{0.0f, 0.0f, 0.0f}, 
										   glm::vec3{-1.0f, 0.0f, 0.0f}, 
										   glm::vec3{0.0f, -1.0f, 0.0f});

	pointLightViewMatrices[2] = glm::lookAt(glm::vec3{0.0f, 0.0f, 0.0f}, 
										   glm::vec3{0.0f, 1.0f, 0.0f}, 
										   glm::vec3{0.0f, 0.0f, 1.0f});

	pointLightViewMatrices[3] = glm::lookAt(glm::vec3{0.0f, 0.0f, 0.0f}, 
											glm::vec3{0.0f, -1.0f, 0.0f}, 
									    	glm::vec3{0.0f, 0.0f, -1.0f});

	pointLightViewMatrices[4] = glm::lookAt(glm::vec3{0.0f, 0.0f, 0.0f}, 
											glm::vec3{0.0f, 0.0f, 1.0f}, 
										    glm::vec3{0.0f, -1.0f, 0.0f});

	pointLightViewMatrices[5] = glm::lookAt(glm::vec3{0.0f, 0.0f, 0.0f}, 
											glm::vec3{0.0f, 0.0f, -1.0f}, 
											glm::vec3{0.0f, -1.0f, 0.0f});

	PointLight* pointLight = &pointLights.emplace_back();
	glm::mat4* pointLightPosMatrix = &pointLightPosMatrices.emplace_back();

	(*pointLight).position = {0.0f, 0.5f, 0.0f};
	(*pointLight).diffuseColor = maxBrightDiffuseColor;
	(*pointLight).specularColor = maxBrightSpecularColor;
	(*pointLight).attenConst = 1.0f;
	(*pointLight).attenLin = 0.045f;
	(*pointLight).attenQuad = 0.0075f;

	*pointLightPosMatrix = glm::translate(glm::mat4{1.0f}, -(*pointLight).position);

	pointLight = &pointLights.emplace_back();
	pointLightPosMatrix = &pointLightPosMatrices.emplace_back();

	(*pointLight).position = {0.0f, 1.0f, 0.0f};
	(*pointLight).diffuseColor = maxBrightDiffuseColor;
	(*pointLight).specularColor = maxBrightSpecularColor;
	(*pointLight).attenConst = 1.0f;
	(*pointLight).attenLin = 0.045f;
	(*pointLight).attenQuad = 0.0075f;

	*pointLightPosMatrix = glm::translate(glm::mat4{1.0f}, -(*pointLight).position);

	glGenBuffers(1, &lightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
	glBufferData(GL_UNIFORM_BUFFER, (MAX_DIR_LIGHTS * sizeof(DirectionalLight)) + (MAX_POINT_LIGHTS * sizeof(PointLight)), 
				 nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, dirLights.size() * sizeof(DirectionalLight), dirLights.data());
	glBufferSubData(GL_UNIFORM_BUFFER, MAX_DIR_LIGHTS * sizeof(DirectionalLight), 
					pointLights.size() * sizeof(PointLight), pointLights.data());

	uniforms.bindUniformBlock(shader, "lights", LIGHTS_UNI_BINDING);
	glBindBufferBase(GL_UNIFORM_BUFFER, LIGHTS_UNI_BINDING, lightsUBO);

	glGenBuffers(1, &numDirLightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, numDirLightsUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
	int* numDirLightsPtr = (int*)glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	*numDirLightsPtr = (int)dirLights.size();
	glUnmapBuffer(GL_UNIFORM_BUFFER);

	uniforms.bindUniformBlock(shader, "numDirLights", NUM_DIRLIGHTS_UNI_BINDING);
	uniforms.bindUniformBlock(dirShadowMapShader, "numDirLights", NUM_DIRLIGHTS_UNI_BINDING);
	glBindBufferBase(GL_UNIFORM_BUFFER, NUM_DIRLIGHTS_UNI_BINDING, numDirLightsUBO);

	glGenBuffers(1, &numPointLightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, numPointLightsUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
	int* numPointLightsPtr = (int*)glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	*numPointLightsPtr = (int)pointLights.size();
	glUnmapBuffer(GL_UNIFORM_BUFFER);

	uniforms.bindUniformBlock(shader, "numPointLights", NUM_POINTLIGHTS_UNI_BINDING);
	uniforms.bindUniformBlock(omniShadowMapShader, "numPointLights", NUM_POINTLIGHTS_UNI_BINDING);
	glBindBufferBase(GL_UNIFORM_BUFFER, NUM_POINTLIGHTS_UNI_BINDING, numPointLightsUBO);

	glGenBuffers(1, &dirLightMatricesUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, dirLightMatricesUBO);
	glBufferData(GL_UNIFORM_BUFFER, MAX_DIR_LIGHTS * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, dirLightMatrices.size() * sizeof(glm::mat4), dirLightMatrices.data());

	uniforms.bindUniformBlock(shader, "dirLightMatrices", DIRLIGHT_MATRICES_UNI_BINDING);
	uniforms.bindUniformBlock(dirShadowMapShader, "dirLightMatrices", DIRLIGHT_MATRICES_UNI_BINDING);
	glBindBufferBase(GL_UNIFORM_BUFFER, DIRLIGHT_MATRICES_UNI_BINDING, dirLightMatricesUBO);

	glGenBuffers(1, &pointLightMatricesUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, pointLightMatricesUBO);
	glBufferData(GL_UNIFORM_BUFFER, (MAX_POINT_LIGHTS + 7) * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, pointLightPosMatrices.size() * sizeof(glm::mat4), pointLightPosMatrices.data());
	glBufferSubData(GL_UNIFORM_BUFFER, MAX_POINT_LIGHTS * sizeof(glm::mat4), sizeof(glm::mat4), 
					&pointLightProjMatrix);
	glBufferSubData(GL_UNIFORM_BUFFER, (MAX_POINT_LIGHTS + 1) * sizeof(glm::mat4), 6 * sizeof(glm::mat4), 
					&pointLightViewMatrices);

	uniforms.bindUniformBlock(omniShadowMapShader, "pointLightMatrices", POINTLIGHT_MATRICES_UNI_BINDING);
	glBindBufferBase(GL_UNIFORM_BUFFER, POINTLIGHT_MATRICES_UNI_BINDING, pointLightMatricesUBO);

	glGenBuffers(1, &pointLightFarPlaneUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, pointLightFarPlaneUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec4), &pointLightFarPlane, GL_STATIC_DRAW);

	uniforms.bindUniformBlock(shader, "pointLightFarPlane", POINTLIGHT_FARPLANE_UNI_BINDING);
	uniforms.bindUniformBlock(omniShadowMapShader, "pointLightFarPlane", POINTLIGHT_FARPLANE_UNI_BINDING);
	glBindBufferBase(GL_UNIFORM_BUFFER, POINTLIGHT_FARPLANE_UNI_BINDING, pointLightFarPlaneUBO);

	TextureParameterSet shadowMapParams;
	shadowMapParams.minFilter = GL_LINEAR;
	shadowMapParams.magFilter = GL_LINEAR;
	shadowMapParams.texWrapS = GL_CLAMP_TO_BORDER;
	shadowMapParams.texWrapT = GL_CLAMP_TO_BORDER;
	shadowMapParams.texWrapR = GL_CLAMP_TO_BORDER;

	dirLightsShadowMapArray = createTexture(GL_TEXTURE_2D_ARRAY, shadowMapParams, GL_DEPTH_COMPONENT16, 
											SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, MAX_DIR_LIGHTS);
	pointLightsShadowMapArray = createCubemap(shadowMapParams, GL_DEPTH_COMPONENT16, 
											  SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, MAX_POINT_LIGHTS);

	glActiveTexture(GL_TEXTURE0 + DIRLIGHT_SHADOW_MAP_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D_ARRAY, dirLightsShadowMapArray);
	glActiveTexture(GL_TEXTURE0 + POINTLIGHT_SHADOW_MAP_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, pointLightsShadowMapArray);

	uniforms.addUniform(shader, "u_dirLightShadowMaps");
	uniforms.addUniform(shader, "u_pointLightShadowMaps");
	uniforms.addUniform(shader, "u_viewPos");
	uniforms.addUniform(shader, "u_ambience");

	glUseProgram(shader);
	uniforms.setUniform(shader, "u_dirLightShadowMaps", DIRLIGHT_SHADOW_MAP_TEXTURE_UNIT);
	uniforms.setUniform(shader, "u_pointLightShadowMaps", POINTLIGHT_SHADOW_MAP_TEXTURE_UNIT); 
	uniforms.setUniform(shader, "u_ambience", ambience);
}

void Application::createPostProcessFBO()
{
	TextureParameterSet texParams =
	{
		.minFilter = GL_LINEAR,
		.magFilter = GL_LINEAR,
		.texWrapS = GL_CLAMP_TO_EDGE,
		.texWrapT = GL_CLAMP_TO_EDGE,
		.texWrapR = GL_CLAMP_TO_EDGE
	};

	glActiveTexture(GL_TEXTURE0 + FB_COLOR_TEXTURE_UNIT);
	fbColorTexture = createTexture(GL_TEXTURE_2D, texParams, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbColorTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glm::vec3 fbQuadPositions[xysquare::NUM_VERTS] =
	{
		{-1.0f, -1.0f, 0.0f}, {-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, -1.0f, 0.0f}
	};

	glm::vec2 fbQuadTexCoords[xysquare::NUM_VERTS] =
	{
		{0.0f, 0.f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}
	};

	using std::byte;

	std::vector<byte> fbQuadVBOVector;
	fbQuadVBOVector.insert(fbQuadVBOVector.end(), (byte*)fbQuadPositions,
						   (byte*)fbQuadPositions + sizeof(fbQuadPositions));
	fbQuadVBOVector.insert(fbQuadVBOVector.end(), (byte*)fbQuadTexCoords,
						   (byte*)fbQuadTexCoords + sizeof(fbQuadTexCoords));

	glGenBuffers(1, &fbQuadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, fbQuadVBO);
	glBufferData(GL_ARRAY_BUFFER, fbQuadVBOVector.size(), fbQuadVBOVector.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &fbQuadEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fbQuadEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(xysquare::indices), xysquare::indices, GL_STATIC_DRAW);

	uniforms.addUniform(fbShader, "fbTexture");

	glUseProgram(fbShader);
	uniforms.setUniform(fbShader, "fbTexture", FB_COLOR_TEXTURE_UNIT);
}

void Application::configureVAO(size_t vboSize)
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindVertexBuffer(FB_POS_BUFFER_BINDING, fbQuadVBO, 0, sizeof(glm::vec3));
	glBindVertexBuffer(FB_TEXCOORD_BUFFER_BINDING, fbQuadVBO, 4 * sizeof(glm::vec3), sizeof(glm::vec2));

	glVertexAttribFormat(VERTEX_POS_ATTRIB_INDEX, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexAttribArray(VERTEX_POS_ATTRIB_INDEX);
	glBindVertexBuffer(OBJ_POS_BUFFER_BINDING, vbo, 0, sizeof(glm::vec3));
	// AttribBinding done in render loop

	glVertexAttribFormat(VERTEX_TEXCOORD_ATTRIB_INDEX, 2, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexAttribArray(VERTEX_TEXCOORD_ATTRIB_INDEX);
	glBindVertexBuffer(OBJ_TEXCOORD_BUFFER_BINDING, vbo, 
					   (size_t)(vboSize * VERTEX_POS_SIZE / VERTEX_TOTAL_SIZE), 
					   sizeof(glm::vec2));
	// AttribBinding done in render loop

	glVertexAttribFormat(VERTEX_NORMAL_ATTRIB_INDEX, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexAttribArray(VERTEX_NORMAL_ATTRIB_INDEX);
	glBindVertexBuffer(OBJ_NORMAL_BUFFER_BINDING, vbo, 
					   (size_t)(vboSize * (VERTEX_POS_SIZE + VERTEX_TEXCOORD_SIZE) / VERTEX_TOTAL_SIZE),
					   sizeof(glm::vec3));
	glVertexAttribBinding(VERTEX_NORMAL_ATTRIB_INDEX, OBJ_NORMAL_BUFFER_BINDING);

	glVertexAttribFormat(VERTEX_TANGENT_ATTRIB_INDEX, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexAttribArray(VERTEX_TANGENT_ATTRIB_INDEX);
	glBindVertexBuffer(OBJ_TANGENT_BUFFER_BINDING, vbo, 
					   (size_t)(vboSize * (VERTEX_POS_SIZE + VERTEX_TEXCOORD_SIZE + VERTEX_NORMAL_SIZE) / VERTEX_TOTAL_SIZE),
					   sizeof(glm::vec3));
	glVertexAttribBinding(VERTEX_TANGENT_ATTRIB_INDEX, OBJ_TANGENT_BUFFER_BINDING);

	glBindVertexBuffer(OBJ_TRANSMAT_BUFFER_BINDING, instanceVBO, 0, sizeof(glm::mat4));
	glVertexBindingDivisor(OBJ_TRANSMAT_BUFFER_BINDING, 1);
	for(int i = 0; i < 4; i++)
	{
		glVertexAttribFormat(i + VERTEX_TRANSMAT_ATTRIB_INDEX, 4, GL_FLOAT, GL_FALSE, i * sizeof(glm::vec4));
		glVertexAttribBinding(i + VERTEX_TRANSMAT_ATTRIB_INDEX, OBJ_TRANSMAT_BUFFER_BINDING);
		glEnableVertexAttribArray(i + VERTEX_TRANSMAT_ATTRIB_INDEX);
	}
}

void Application::miscInit()
{
	glGenBuffers(1, &matsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, matsUBO);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera.projectionMatrix()));

	uniforms.bindUniformBlock(shader, "mats", CAM_MATS_UNI_BINDING);
	uniforms.bindUniformBlock(skyboxShader, "mats", CAM_MATS_UNI_BINDING);
	glBindBufferBase(GL_UNIFORM_BUFFER, CAM_MATS_UNI_BINDING, matsUBO);
}

Application::Application() :
	SCREEN_WIDTH{1280}, SCREEN_HEIGHT{720},
	window{initialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Advanced OpenGL", 4, 6)},
	keys{window}, mouse{window},

	camera{glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f},
	camSensitivity{1.0f}, camSpeed{15.0f}, pointLightBrightnessSpeed{0.5f}, pointLightMoveSpeed{15.0f},

	shader{createShaderProgram("../src/shaders/object_vs.glsl", "../src/shaders/object_fs.glsl")},
	fbShader{createShaderProgram("../src/shaders/framebuffer_vs.glsl", "../src/shaders/framebuffer_fs.glsl")},
	skyboxShader{createShaderProgram("../src/shaders/skybox_vs.glsl", "../src/shaders/skybox_fs.glsl")},
	dirShadowMapShader{createShaderProgram("../src/shaders/dir_shadow_map_vs.glsl",
										   "../src/shaders/dir_shadow_map_gs.glsl",
										   "../src/shaders/dir_shadow_map_fs.glsl")},
	omniShadowMapShader{createShaderProgram("../src/shaders/omni_shadow_map_vs.glsl",
											"../src/shaders/omni_shadow_map_gs.glsl",
										   "../src/shaders/omni_shadow_map_fs.glsl")}
{
	glDebugMessageCallback(glDebugCallback, nullptr);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_DEBUG_OUTPUT);
	glDepthFunc(GL_LEQUAL);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	keys.setKeybind("FORWARD", GLFW_KEY_W);
	keys.setKeybind("BACKWARD", GLFW_KEY_S);
	keys.setKeybind("LEFT", GLFW_KEY_A);
	keys.setKeybind("RIGHT", GLFW_KEY_D);
	keys.setKeybind("UP", GLFW_KEY_LEFT_SHIFT);
	keys.setKeybind("DOWN", GLFW_KEY_LEFT_CONTROL);
	keys.setKeybind("LIGHT_BRIGHT", GLFW_KEY_RIGHT_BRACKET);
	keys.setKeybind("LIGHT_DIM", GLFW_KEY_LEFT_BRACKET);
	keys.setKeybind("LIGHT_UP", GLFW_KEY_O);
	keys.setKeybind("LIGHT_DOWN", GLFW_KEY_L);

	loadTextures();
	size_t vboSize = loadModels();
	createObjects();
	createLightSources();
	createPostProcessFBO();
	configureVAO(vboSize);
	miscInit();
}

Application::~Application()
{
	// Check properly what to delete

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &instanceVBO);
	glDeleteBuffers(1, &matsUBO);
	glDeleteTextures(1, &skybox);
	glDeleteProgram(shader);
	glDeleteProgram(fbShader);
	glDeleteProgram(skyboxShader);
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &fbColorTexture);
	glDeleteRenderbuffers(1, &rbo);
	glDeleteBuffers(1, &fbQuadVBO);
	glDeleteBuffers(1, &fbQuadEBO);
}

void Application::run()
{
	glfwPollEvents();
	keys.update();
	mouse.update();

	printf("Starting render loop.\n");

	while(!glfwWindowShouldClose(window))
	{
		float currentTime = (float)glfwGetTime();
		float deltaTime = currentTime - prevTime;
		prevTime = currentTime;

		glfwPollEvents();
		keys.update();
		mouse.update();

		if(keys.keyPressed("FORWARD"))
		{
			camera.position += -camera.behind() * camSpeed * deltaTime;
		}
		else if(keys.keyPressed("BACKWARD"))
		{
			camera.position += camera.behind() * camSpeed * deltaTime;
		}
		if(keys.keyPressed("LEFT"))
		{
			camera.position += -camera.right() * camSpeed * deltaTime;
		}
		else if(keys.keyPressed("RIGHT"))
		{
			camera.position += camera.right() * camSpeed * deltaTime;
		}
		if(keys.keyPressed("UP"))
		{
			camera.position += camera.up() * camSpeed * deltaTime;
		}
		else if(keys.keyPressed("DOWN"))
		{
			camera.position += -camera.up() * camSpeed * deltaTime;
		}
		
		glm::vec3 camUp{0.0f, 1.0f, 0.0f};
		glm::vec3 camRight{camera.right()};
		
		float mouseMoveX = mouse.getMouseMovementX();
		if(mouseMoveX != 0)
		{
			camera.rotateGlobal(camUp, -mouseMoveX * camSensitivity * deltaTime);
		}

		float mouseMoveY = mouse.getMouseMovementY();
		if(mouseMoveY != 0)
		{
			camera.rotateGlobal(camRight, mouseMoveY * camSensitivity * deltaTime);
		}

		if(keys.keyPressed("LIGHT_BRIGHT") && 
		   (glm::length(pointLights[0].diffuseColor) < glm::length(maxBrightDiffuseColor)))
		{
			pointLights[0].diffuseColor += pointLightBrightnessSpeed * maxBrightDiffuseColor * deltaTime;
			pointLights[0].specularColor += pointLightBrightnessSpeed * maxBrightSpecularColor * deltaTime;
			lightCubeEmissiveStrength += pointLightBrightnessSpeed * deltaTime; 
		}
		else if(keys.keyPressed("LIGHT_DIM") && (glm::length(pointLights[0].diffuseColor) > 0.1f))
		{
			pointLights[0].diffuseColor -= pointLightBrightnessSpeed * maxBrightDiffuseColor * deltaTime;
			pointLights[0].specularColor -= pointLightBrightnessSpeed * maxBrightSpecularColor * deltaTime;
			lightCubeEmissiveStrength -= pointLightBrightnessSpeed * deltaTime;
		}

		glm::vec3 pointLightMove{};
		if(keys.keyPressed("LIGHT_UP"))
		{
			pointLightMove.y = pointLightMoveSpeed * deltaTime;
		}
		else if(keys.keyPressed("LIGHT_DOWN"))
		{
			pointLightMove.y = -pointLightMoveSpeed * deltaTime;
		}
		pointLights[0].position += pointLightMove;
		pointLightPosMatrices[0] = glm::translate(glm::mat4{1.0f}, -pointLights[0].position);

		glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, MAX_DIR_LIGHTS * sizeof(DirectionalLight), sizeof(PointLight), &pointLights[0]);

		glBindBuffer(GL_UNIFORM_BUFFER, pointLightMatricesUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, pointLightPosMatrices.size() * sizeof(glm::mat4), pointLightPosMatrices.data());

		lightCubeTransformMat = glm::translate(glm::mat4{1.0f}, pointLightMove) * lightCubeTransformMat;

		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferSubData(GL_ARRAY_BUFFER, lightCubeModelInfo.instanceOffset * sizeof(glm::mat4), sizeof(glm::mat4), 
						glm::value_ptr(lightCubeTransformMat));

		//----------------------------------------

		glBindFramebuffer(GL_FRAMEBUFFER, textureWriteFBO);
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
		glVertexAttribBinding(VERTEX_POS_ATTRIB_INDEX, OBJ_POS_BUFFER_BINDING);
		glVertexAttribBinding(VERTEX_TEXCOORD_ATTRIB_INDEX, OBJ_TEXCOORD_BUFFER_BINDING);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glEnable(GL_CULL_FACE);

		glUseProgram(dirShadowMapShader);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, dirLightsShadowMapArray, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		glClear(GL_DEPTH_BUFFER_BIT);

		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, woodCubeModelInfo.indexCount, GL_UNSIGNED_INT,
													  (const void*)(woodCubeModelInfo.eboOffset * sizeof(unsigned int)), 
													  woodCubeModelInfo.instanceCount,
													  woodCubeModelInfo.vboOffset, woodCubeModelInfo.instanceOffset);
		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, floorModelInfo.indexCount, GL_UNSIGNED_INT,
													  (const void*)(floorModelInfo.eboOffset * sizeof(unsigned int)), 
													  floorModelInfo.instanceCount,
													  floorModelInfo.vboOffset, floorModelInfo.instanceOffset);

		glUseProgram(omniShadowMapShader);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, pointLightsShadowMapArray, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		glClear(GL_DEPTH_BUFFER_BIT);

		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, woodCubeModelInfo.indexCount, GL_UNSIGNED_INT,
													  (const void*)(woodCubeModelInfo.eboOffset * sizeof(unsigned int)), 
													  woodCubeModelInfo.instanceCount,
													  woodCubeModelInfo.vboOffset, woodCubeModelInfo.instanceOffset);
		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, floorModelInfo.indexCount, GL_UNSIGNED_INT,
													  (const void*)(floorModelInfo.eboOffset * sizeof(unsigned int)), 
													  floorModelInfo.instanceCount,
													  floorModelInfo.vboOffset, floorModelInfo.instanceOffset);

		//----------------------------------------

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		glm::mat4 viewMatrix = camera.viewMatrix();
		glBindBuffer(GL_UNIFORM_BUFFER, matsUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMatrix));

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader);
		uniforms.setUniform(shader, "u_viewPos", camera.position);
		
		glActiveTexture(GL_TEXTURE0 + EMISSIVE_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, woodCubeModelInfo.emissiveMapID);
		glActiveTexture(GL_TEXTURE0 + DIFFUSE_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, woodCubeModelInfo.diffuseMapID);
		glActiveTexture(GL_TEXTURE0 + SPECULAR_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, woodCubeModelInfo.specularMapID);
		glActiveTexture(GL_TEXTURE0 + NORMAL_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, woodCubeModelInfo.normalMapID);
		glActiveTexture(GL_TEXTURE0 + DISPLACEMENT_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, woodCubeModelInfo.dispMapID);

		uniforms.setUniform(shader, "u_materialShininess", cubeShininess);
		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, woodCubeModelInfo.indexCount, GL_UNSIGNED_INT,
													  (const void*)(woodCubeModelInfo.eboOffset * sizeof(unsigned int)), 
													  woodCubeModelInfo.instanceCount,
													  woodCubeModelInfo.vboOffset, woodCubeModelInfo.instanceOffset);

		glActiveTexture(GL_TEXTURE0 + EMISSIVE_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, lightCubeModelInfo.emissiveMapID);
		glActiveTexture(GL_TEXTURE0 + DIFFUSE_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, lightCubeModelInfo.diffuseMapID);
		glActiveTexture(GL_TEXTURE0 + SPECULAR_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, lightCubeModelInfo.specularMapID);
		glActiveTexture(GL_TEXTURE0 + NORMAL_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, lightCubeModelInfo.normalMapID);
		glActiveTexture(GL_TEXTURE0 + DISPLACEMENT_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, lightCubeModelInfo.dispMapID);

		uniforms.setUniform(shader, "u_emissiveStrength", lightCubeEmissiveStrength);
		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, lightCubeModelInfo.indexCount, GL_UNSIGNED_INT,
													  (const void*)(lightCubeModelInfo.eboOffset * sizeof(unsigned int)), 
													  lightCubeModelInfo.instanceCount,
													  lightCubeModelInfo.vboOffset, lightCubeModelInfo.instanceOffset);

		glActiveTexture(GL_TEXTURE0 + EMISSIVE_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, floorModelInfo.emissiveMapID);
		glActiveTexture(GL_TEXTURE0 + DIFFUSE_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, floorModelInfo.diffuseMapID);
		glActiveTexture(GL_TEXTURE0 + SPECULAR_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, floorModelInfo.specularMapID);
		glActiveTexture(GL_TEXTURE0 + NORMAL_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, floorModelInfo.normalMapID);
		glActiveTexture(GL_TEXTURE0 + DISPLACEMENT_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, floorModelInfo.dispMapID);

		uniforms.setUniform(shader, "u_materialShininess", floorShininess);
		uniforms.setUniform(shader, "u_heightScale", floorHeightScale);
		uniforms.setUniform(shader, "u_numDispLayers", numDispLayers);
		glDisable(GL_CULL_FACE);
		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, floorModelInfo.indexCount, GL_UNSIGNED_INT,
													  (const void*)(floorModelInfo.eboOffset * sizeof(unsigned int)), 
													  floorModelInfo.instanceCount,
													  floorModelInfo.vboOffset, floorModelInfo.instanceOffset);

		glUseProgram(skyboxShader);
		glDrawElements(GL_TRIANGLES, cube::NUM_INDICES, GL_UNSIGNED_INT, (const void*)0);

		//------------------------------------

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);

		glUseProgram(fbShader);

		glVertexAttribBinding(VERTEX_POS_ATTRIB_INDEX, FB_POS_BUFFER_BINDING);
		glVertexAttribBinding(VERTEX_TEXCOORD_ATTRIB_INDEX, FB_TEXCOORD_BUFFER_BINDING);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fbQuadEBO);

		glDrawElements(GL_TRIANGLES, xysquare::NUM_INDICES, GL_UNSIGNED_INT, (const void*)0);

		glfwSwapBuffers(window);
	}
}