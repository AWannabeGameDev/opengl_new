#include "application.h"
#include <utility/raw_model_data.h>
#include <ctime>
#include <utility/stb_image.h>
#include <format>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "struct_uniform_util.h"

using namespace models;

unsigned int createTexture(unsigned int target, const TextureParameterSet& texParams, 
						   unsigned int format, std::string_view path, bool flip, 
						   int width = 0, int height = 0)
{
	unsigned int texID;
	glGenTextures(1, &texID);
	glBindTexture(target, texID);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, texParams.minFilter);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, texParams.magFilter);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, texParams.texWrapS);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, texParams.texWrapT);
	glTexParameteri(target, GL_TEXTURE_WRAP_R, texParams.texWrapR);

	if((width == 0) && (height == 0))
	{
		TextureData texInfo{loadTexture(path, flip)};
		glTexImage2D(target, 0, format, texInfo.width, texInfo.height, 0, texInfo.format, GL_UNSIGNED_BYTE, texInfo.data);
		stbi_image_free(texInfo.data);
	}
	else
	{
		glTexImage2D(target, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
	}

	if((texParams.minFilter >= GL_NEAREST_MIPMAP_NEAREST) && (texParams.minFilter <= GL_LINEAR_MIPMAP_LINEAR))
	{
		glGenerateMipmap(target);
	}

	return texID;
}

unsigned int createCubemap(const TextureParameterSet& texParams, 
						   unsigned int format, std::string_view paths[6], bool flip,
						   int width = 0, int height = 0)
{
	unsigned int texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, texParams.minFilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, texParams.magFilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, texParams.texWrapS);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, texParams.texWrapT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, texParams.texWrapR);

	if((width == 0) && (height == 0))
	{
		for(size_t i = 0; i < 6; i++)
		{
			TextureData texInfo{loadTexture(paths[i], flip)};
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, 
						 texInfo.width, texInfo.height, 0, texInfo.format, GL_UNSIGNED_BYTE, texInfo.data);
			stbi_image_free(texInfo.data);
		}
	}
	else
	{
		for(unsigned int i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, 
						 width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
		}
	}

	if((texParams.minFilter >= GL_NEAREST_MIPMAP_NEAREST) && (texParams.minFilter <= GL_LINEAR_MIPMAP_LINEAR))
	{
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	return texID;
}

void Application::addLightUniforms()
{
	addDirLightUniforms(uniforms, shader, "u_dirLight");
	uniforms.addUniform(dirShadowMapShader, "u_lightSpaceMatrix");
	uniforms.addUniform(shader, "u_dirLightSpaceMatrix");
	uniforms.addUniform(shader, "u_dirLightShadowMap");
	uniforms.addUniform(shader, "u_pointLightFarPlane");

	addPointLightUniforms(uniforms, shader, "u_pointLight");
	uniforms.addUniform(omniShadowMapShader, "u_lightSpaceViewMatrices[0]");
	uniforms.addUniform(omniShadowMapShader, "u_lightSpaceViewMatrices[1]");
	uniforms.addUniform(omniShadowMapShader, "u_lightSpaceViewMatrices[2]");
	uniforms.addUniform(omniShadowMapShader, "u_lightSpaceViewMatrices[3]");
	uniforms.addUniform(omniShadowMapShader, "u_lightSpaceViewMatrices[4]");
	uniforms.addUniform(omniShadowMapShader, "u_lightSpaceViewMatrices[5]");
	uniforms.addUniform(omniShadowMapShader, "u_lightSpaceProjMatrix");
	uniforms.addUniform(omniShadowMapShader, "u_lightSpacePositionMatrix");
	uniforms.addUniform(omniShadowMapShader, "u_farPlane");
	uniforms.addUniform(shader, "u_pointLightShadowMap");

	uniforms.addUniform(shader, "u_viewPos");
	uniforms.addUniform(shader, "u_ambience");
}

void Application::addTextureUniforms()
{
	uniforms.addUniform(shader, "u_diffuse");
	uniforms.addUniform(shader, "u_specular");
	uniforms.addUniform(shader, "u_emissive");
	uniforms.addUniform(shader, "u_normal");
	uniforms.addUniform(shader, "u_displacement");
	uniforms.addUniform(shader, "u_materialShininess");
	uniforms.addUniform(shader, "u_heightScale");
	uniforms.addUniform(shader, "u_emissiveStrength");

	uniforms.addUniform(skyboxShader, "u_skybox");
	uniforms.addUniform(fbShader, "fbTexture");
}

void Application::initLightStructsAndMatrices()
{
	ambience = 0.1f;

	dirLightRender.source.direction = glm::normalize(glm::vec3{1.0f, -1.0f, 1.0f});
	dirLightRender.source.diffuseColor = 0.0f * glm::vec3{0.6f, 0.6f, 0.6f};
	dirLightRender.source.specularColor = 0.0f * glm::vec3{0.3f, 0.3f, 0.3f};

	dirLightRender.matrix = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, -15.0f, 15.0f) *
		glm::lookAt(-dirLightRender.source.direction, glm::vec3{0.0f}, glm::vec3{0.0f, 1.0f, 0.0f});

	maxBrightDiffuseColor = {0.0f, 0.9f, 0.9f};
	maxBrightSpecularColor = {0.0f, 0.5f, 0.5f};

	pointLightRender.source.position = {0.0f, 2.0f, 0.0f};
	pointLightRender.source.diffuseColor = maxBrightDiffuseColor;
	pointLightRender.source.specularColor = maxBrightSpecularColor;
	pointLightRender.source.attenConst = 1.0f;
	pointLightRender.source.attenLin = 0.045f;
	pointLightRender.source.attenQuad = 0.0075f;

	pointLightRender.farPlane = 20.0f;
	pointLightRender.projMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, pointLightRender.farPlane);
	pointLightRender.positionMatrix = glm::translate(glm::mat4{1.0f}, -pointLightRender.source.position);

	pointLightRender.viewMatrices[0] = glm::lookAt(glm::vec3{0.0f, 0.0f, 0.0f}, 
												   glm::vec3{1.0f, 0.0f, 0.0f}, 
												   glm::vec3{0.0f, -1.0f, 0.0f});

	pointLightRender.viewMatrices[1] = glm::lookAt(glm::vec3{0.0f, 0.0f, 0.0f}, 
												   glm::vec3{-1.0f, 0.0f, 0.0f}, 
												   glm::vec3{0.0f, -1.0f, 0.0f});

	pointLightRender.viewMatrices[2] = glm::lookAt(glm::vec3{0.0f, 0.0f, 0.0f}, 
												   glm::vec3{0.0f, 1.0f, 0.0f}, 
												   glm::vec3{0.0f, 0.0f, 1.0f});

	pointLightRender.viewMatrices[3] = glm::lookAt(glm::vec3{0.0f, 0.0f, 0.0f}, 
												   glm::vec3{0.0f, -1.0f, 0.0f}, 
												   glm::vec3{0.0f, 0.0f, -1.0f});

	pointLightRender.viewMatrices[4] = glm::lookAt(glm::vec3{0.0f, 0.0f, 0.0f}, 
												   glm::vec3{0.0f, 0.0f, 1.0f}, 
												   glm::vec3{0.0f, -1.0f, 0.0f});

	pointLightRender.viewMatrices[5] = glm::lookAt(glm::vec3{0.0f, 0.0f, 0.0f}, 
												   glm::vec3{0.0f, 0.0f, -1.0f}, 
												   glm::vec3{0.0f, -1.0f, 0.0f});
}

void Application::createLightShadowMaps()
{
	TextureParameterSet shadowMapParams;
	shadowMapParams.minFilter = GL_LINEAR;
	shadowMapParams.magFilter = GL_LINEAR;
	shadowMapParams.texWrapS = GL_CLAMP_TO_BORDER;
	shadowMapParams.texWrapT = GL_CLAMP_TO_BORDER;
	shadowMapParams.texWrapR = GL_CLAMP_TO_BORDER;

	dirLightRender.shadowMap = createTexture(GL_TEXTURE_2D, shadowMapParams, GL_DEPTH_COMPONENT, "", false, 
											 SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
	pointLightRender.shadowCubeMap = createCubemap(shadowMapParams, GL_DEPTH_COMPONENT, {}, false, 
												   SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
}

size_t Application::createVBO()
{
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
		//{floorWidth / 8, floorWidth / 8}, {0.0f, floorWidth / 8}, {0.0f, 0.0f}, {floorWidth / 8, 0.0f}
		{floorWidth / 8.0f, floorWidth / 8.0f}, {0.0f, floorWidth / 8.0f}, {0.0f, 0.0f}, {floorWidth / 8.0f, 0.0f}
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

	return vboVector.size();
}

void Application::createEBO()
{
	using std::byte;

	std::vector<byte> eboVector;
	eboVector.insert(eboVector.end(), (byte*)cube::indices, (byte*)cube::indices + sizeof(cube::indices));
	eboVector.insert(eboVector.end(), (byte*)xysquare::indices, (byte*)xysquare::indices + sizeof(xysquare::indices));

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboVector.size(), eboVector.data(), GL_STATIC_DRAW);
}	 

void Application::initObjectTransforms()
{
	floorWidth = 20.0f;

	srand((unsigned int)time(0));
	for(glm::mat4& cubeTransformMat : cubeTransformMats)
	{
		Transform cubeTransform{};
		cubeTransform.position = {randrange(-9.5f, 9.5f), 0.50f, randrange(-9.5f, 9.5f)};
		cubeTransformMat = cubeTransform.matrix();
	}
	Transform lightCubeTransform{};
	lightCubeTransform.scale = {0.25f, 0.25f, 0.25f};
	lightCubeTransform.position = pointLightRender.source.position;
	lightCubeTransformMat = lightCubeTransform.matrix();

	Transform floorTransform{};
	floorTransform.scale = {floorWidth, floorWidth, 1.0f};
	floorTransform.rotation = glm::angleAxis(glm::radians(-90.0f), glm::vec3{1.0f, 0.0f, 0.0f});
	floorTransformMat = floorTransform.matrix();
}

void Application::createObjectTransformVBO()
{
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

void Application::createMatrixUBO()
{
	matsUniformBinding = 0;

	glGenBuffers(1, &matsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, matsUBO);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera.projectionMatrix()));

	glUniformBlockBinding(shader, glGetUniformBlockIndex(shader, "mats"), matsUniformBinding);
	glUniformBlockBinding(skyboxShader, glGetUniformBlockIndex(skyboxShader, "mats"), matsUniformBinding);
	glBindBufferBase(GL_UNIFORM_BUFFER, matsUniformBinding, matsUBO);
}

void Application::createTextureMaps()
{
	cubeShininess = 128.0f; 
	floorShininess = 16.0f;
	lightCubeEmissiveStrength = 1.0f;
	floorHeightScale = 0.6f;

	TextureParameterSet texParams;
	texParams.minFilter = GL_LINEAR_MIPMAP_LINEAR;
	texParams.magFilter = GL_LINEAR;
	texParams.texWrapS = GL_REPEAT;
	texParams.texWrapT = GL_REPEAT;
	texParams.texWrapR = GL_REPEAT;

	cubeDiffuseTexture = createTexture(GL_TEXTURE_2D, texParams, GL_SRGB, "../res/container_diffuse.png", true);
	cubeSpecularTexture = createTexture(GL_TEXTURE_2D, texParams, GL_RGB, "../res/container_specular.png", true);
	floorDiffuseTexture = createTexture(GL_TEXTURE_2D, texParams, GL_SRGB, "../res/bricks2_diffuse.jpg", true);
	//floorSpecularTexture = createTexture(GL_TEXTURE_2D, texParams, GL_RGB, "../res/brick_specular.png", true);
	floorNormalTexture = createTexture(GL_TEXTURE_2D, texParams, GL_RGB, "../res/bricks2_normal.jpg", true);
	floorDispTexture = createTexture(GL_TEXTURE_2D, texParams, GL_RGB, "../res/bricks2_displacement.jpg", true);

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

	blackTexture = createTexture(GL_TEXTURE_2D, texParams, GL_RGB, "", false, 1, 1);
	whiteTexture = createTexture(GL_TEXTURE_2D, texParams, GL_RGB, "", false, 1, 1);
	lightCubeEmissiveTexture = createTexture(GL_TEXTURE_2D, texParams, GL_RGB, "", false, 1, 1);
	defaultNormalTexture = createTexture(GL_TEXTURE_2D, texParams, GL_RGB, "", false, 1, 1);

	glGenFramebuffers(1, &textureWriteFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, textureWriteFBO);
	glViewport(0, 0, 1, 1);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightCubeEmissiveTexture, 0);
	glClearColor(pointLightRender.source.diffuseColor.r,
				 pointLightRender.source.diffuseColor.g, pointLightRender.source.diffuseColor.b, 0.0f);
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
}

void Application::createPostProcessFBO()
{
	glGenTextures(1, &fbColorTexture);
	glActiveTexture(GL_TEXTURE0 + FB_COLOR_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, fbColorTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

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
}

void Application::setVAOFormatsAndBuffers(size_t vboSize)
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

void Application::setUniforms()
{
	glUseProgram(shader);
	setDirLightUniform(uniforms, shader, "u_dirLight", dirLightRender.source); // Should be done in render loop if dynamic
	setPointLightUniform(uniforms, shader, "u_pointLight", pointLightRender.source); // Should be done in 
																					 // render loop if dynamic
	uniforms.setUniform(shader, "u_ambience", ambience);
	uniforms.setUniform(shader, "u_diffuse", DIFFUSE_TEXTURE_UNIT);   // These five can
	uniforms.setUniform(shader, "u_specular", SPECULAR_TEXTURE_UNIT); // have texture-arrays
	uniforms.setUniform(shader, "u_emissive", EMISSIVE_TEXTURE_UNIT); // of the same size
	uniforms.setUniform(shader, "u_normal", NORMAL_TEXTURE_UNIT);
	uniforms.setUniform(shader, "u_displacement", DISPLACEMENT_TEXTURE_UNIT);
	uniforms.setUniform(shader, "u_dirLightShadowMap", SHADOW_MAP_TEXTURE_UNIT); // Texture-arrays probably
	uniforms.setUniform(shader, "u_pointLightShadowMap", SHADOW_MAP_TEXTURE_UNIT + 1); // Texture-arrays probably

	glUseProgram(omniShadowMapShader);
	uniforms.setUniform(omniShadowMapShader, "u_lightSpaceProjMatrix", pointLightRender.projMatrix);
	for(int i = 0; i < 6; i++)
	{
		uniforms.setUniform(omniShadowMapShader, std::format("u_lightSpaceViewMatrices[{}]", i), 
							pointLightRender.viewMatrices[i]);
	}
	uniforms.setUniform(omniShadowMapShader, "u_farPlane", pointLightRender.farPlane);

	glUseProgram(skyboxShader);
	uniforms.setUniform(skyboxShader, "u_skybox", SKYBOX_TEXTURE_UNIT);

	glUseProgram(fbShader);
	uniforms.setUniform(fbShader, "fbTexture", FB_COLOR_TEXTURE_UNIT);
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
	keys.setKeybind("UP", GLFW_KEY_SPACE);
	keys.setKeybind("DOWN", GLFW_KEY_LEFT_SHIFT);
	keys.setKeybind("LIGHT_BRIGHT", GLFW_KEY_RIGHT_BRACKET);
	keys.setKeybind("LIGHT_DIM", GLFW_KEY_LEFT_BRACKET);
	keys.setKeybind("LIGHT_UP", GLFW_KEY_O);
	keys.setKeybind("LIGHT_DOWN", GLFW_KEY_L);

	addTextureUniforms();
	addLightUniforms();

	initLightStructsAndMatrices();
	initObjectTransforms();
	
	createObjectTransformVBO();
	size_t vboSize = createVBO();
	createEBO();

	createMatrixUBO();
	createLightShadowMaps();
	createTextureMaps();

	createPostProcessFBO();

	setVAOFormatsAndBuffers(vboSize);

	setUniforms();
}

Application::~Application()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &instanceVBO);
	glDeleteBuffers(1, &matsUBO);
	glDeleteTextures(1, &cubeDiffuseTexture);
	glDeleteTextures(1, &floorDiffuseTexture);
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
		   (glm::length(pointLightRender.source.diffuseColor) < glm::length(maxBrightDiffuseColor)))
		{
			pointLightRender.source.diffuseColor += pointLightBrightnessSpeed * maxBrightDiffuseColor * deltaTime;
			pointLightRender.source.specularColor += pointLightBrightnessSpeed * maxBrightSpecularColor * deltaTime;
			lightCubeEmissiveStrength += pointLightBrightnessSpeed * deltaTime; 
		}
		else if(keys.keyPressed("LIGHT_DIM") && (glm::length(pointLightRender.source.diffuseColor) > 0.1f))
		{
			pointLightRender.source.diffuseColor -= pointLightBrightnessSpeed * maxBrightDiffuseColor * deltaTime;
			pointLightRender.source.specularColor -= pointLightBrightnessSpeed * maxBrightSpecularColor * deltaTime;
			lightCubeEmissiveStrength -= pointLightBrightnessSpeed * deltaTime;
		}

		if(keys.keyPressed("LIGHT_UP"))
		{
			pointLightRender.source.position.y += pointLightMoveSpeed * deltaTime;
		}
		else if(keys.keyPressed("LIGHT_DOWN"))
		{
			pointLightRender.source.position.y -= pointLightMoveSpeed * deltaTime; 
		}
		pointLightRender.positionMatrix = glm::translate(glm::mat4{1.0f}, -pointLightRender.source.position);

		//----------------------------------------

		glBindFramebuffer(GL_FRAMEBUFFER, textureWriteFBO);
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
		glVertexAttribBinding(VERTEX_POS_ATTRIB_INDEX, OBJ_POS_BUFFER_BINDING);
		glVertexAttribBinding(VERTEX_TEXCOORD_ATTRIB_INDEX, OBJ_TEXCOORD_BUFFER_BINDING);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glEnable(GL_CULL_FACE);

		glUseProgram(dirShadowMapShader);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, dirLightRender.shadowMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		uniforms.setUniform(dirShadowMapShader, "u_lightSpaceMatrix", dirLightRender.matrix);

		glClear(GL_DEPTH_BUFFER_BIT);

		glDrawElementsInstanced(GL_TRIANGLES, cube::NUM_INDICES, GL_UNSIGNED_INT, (const void*)0, NUM_CUBES);
		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, xysquare::NUM_INDICES, GL_UNSIGNED_INT,
													  (const void*)(cube::NUM_INDICES * sizeof(unsigned int)),
													  1, cube::NUM_VERTS, NUM_CUBES + 1);

		glUseProgram(omniShadowMapShader);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, pointLightRender.shadowCubeMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		uniforms.setUniform(omniShadowMapShader, "u_lightSpacePositionMatrix", pointLightRender.positionMatrix);

		glClear(GL_DEPTH_BUFFER_BIT);

		glDrawElementsInstanced(GL_TRIANGLES, cube::NUM_INDICES, GL_UNSIGNED_INT, (const void*)0, NUM_CUBES);
		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, xysquare::NUM_INDICES, GL_UNSIGNED_INT,
													  (const void*)(cube::NUM_INDICES * sizeof(unsigned int)),
													  1, cube::NUM_VERTS, NUM_CUBES + 1);

		//----------------------------------------

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		glm::mat4 viewMatrix = camera.viewMatrix();
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMatrix));

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader);
		uniforms.setUniform(shader, "u_viewPos", camera.position);
		uniforms.setUniform(shader, "u_dirLightSpaceMatrix", dirLightRender.matrix);
		uniforms.setUniform(shader, "u_pointLightFarPlane", pointLightRender.farPlane);
		uniforms.setUniform(shader, "u_pointLight.diffuseColor", pointLightRender.source.diffuseColor);
		uniforms.setUniform(shader, "u_pointLight.specularColor", pointLightRender.source.specularColor);
		uniforms.setUniform(shader, "u_pointLight.pos", pointLightRender.source.position);

		glActiveTexture(GL_TEXTURE0 + SHADOW_MAP_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, dirLightRender.shadowMap);
		glActiveTexture(GL_TEXTURE0 + SHADOW_MAP_TEXTURE_UNIT + 1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, pointLightRender.shadowCubeMap);
		
		glActiveTexture(GL_TEXTURE0 + EMISSIVE_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, blackTexture);
		glActiveTexture(GL_TEXTURE0 + DIFFUSE_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, cubeDiffuseTexture);
		glActiveTexture(GL_TEXTURE0 + SPECULAR_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, cubeSpecularTexture);
		glActiveTexture(GL_TEXTURE0 + NORMAL_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, defaultNormalTexture);
		glActiveTexture(GL_TEXTURE0 + DISPLACEMENT_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, blackTexture);

		uniforms.setUniform(shader, "u_materialShininess", cubeShininess);
		glDrawElementsInstanced(GL_TRIANGLES, cube::NUM_INDICES, GL_UNSIGNED_INT, (const void*)0, NUM_CUBES);

		glActiveTexture(GL_TEXTURE0 + EMISSIVE_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, lightCubeEmissiveTexture);
		glActiveTexture(GL_TEXTURE0 + DIFFUSE_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, blackTexture);
		glActiveTexture(GL_TEXTURE0 + SPECULAR_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, blackTexture);
		glActiveTexture(GL_TEXTURE0 + NORMAL_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, defaultNormalTexture);
		glActiveTexture(GL_TEXTURE0 + DISPLACEMENT_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, blackTexture);

		// TODO: Update vertex data of light cube position

		uniforms.setUniform(shader, "u_emissiveStrength", lightCubeEmissiveStrength);
		glDrawElementsInstancedBaseInstance(GL_TRIANGLES, cube::NUM_INDICES, GL_UNSIGNED_INT, (const void*)0, 1, NUM_CUBES);

		glActiveTexture(GL_TEXTURE0 + EMISSIVE_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, blackTexture);
		glActiveTexture(GL_TEXTURE0 + DIFFUSE_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, floorDiffuseTexture);
		glActiveTexture(GL_TEXTURE0 + SPECULAR_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, whiteTexture);
		glActiveTexture(GL_TEXTURE0 + NORMAL_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, floorNormalTexture);
		glActiveTexture(GL_TEXTURE0 + DISPLACEMENT_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, floorDispTexture);

		uniforms.setUniform(shader, "u_materialShininess", floorShininess);
		uniforms.setUniform(shader, "u_heightScale", floorHeightScale);
		glDisable(GL_CULL_FACE);
		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, xysquare::NUM_INDICES, GL_UNSIGNED_INT, 
													  (const void*)(cube::NUM_INDICES * sizeof(unsigned int)), 
													  1, cube::NUM_VERTS, NUM_CUBES + 1);

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