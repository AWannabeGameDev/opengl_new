#include "application.h"
#include <utility/raw_model_data.h>
#include <ctime>
#include <utility/stb_image.h>
#include <format>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "struct_uniform_util.h"

using namespace models;

void texImageForSkybox(GLenum target, std::string_view path)
{
	TextureData skyboxTexData = loadTexture(path, false);
	glTexImage2D(target, 0, GL_SRGB, skyboxTexData.width, skyboxTexData.height, 0, 
				 skyboxTexData.format, GL_UNSIGNED_BYTE, skyboxTexData.data);
	stbi_image_free(skyboxTexData.data);
}

unsigned int createObjectTexture(std::string_view path, GLenum format)
{
	unsigned int texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	TextureData texData = loadTexture(path, true);
	glTexImage2D(GL_TEXTURE_2D, 0, format, texData.width, texData.height, 
				 0, texData.format, GL_UNSIGNED_BYTE, texData.data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(texData.data);

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
	uniforms.addUniform(shader, "u_materialShininess");

	uniforms.addUniform(skyboxShader, "u_skybox");
	uniforms.addUniform(fbShader, "fbTexture");
}

void Application::initLightStructsAndMatrices()
{
	dirLightRender.source.direction = glm::normalize(glm::vec3{1.0f, -1.0f, 1.0f});
	dirLightRender.source.diffuseColor = glm::vec3{0.8f, 0.8f, 0.8f};
	dirLightRender.source.specularColor = glm::vec3{0.2f, 0.2f, 0.2f};

	dirLightRender.matrix = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, -15.0f, 15.0f) *
		glm::lookAt(-dirLightRender.source.direction, glm::vec3{0.0f}, glm::vec3{0.0f, 1.0f, 0.0f});

	pointLightRender.source.position = {0.0f, 2.0f, 0.0f};
	pointLightRender.source.diffuseColor = glm::vec3{1.0f, 0.34f, 0.2f};
	pointLightRender.source.specularColor = glm::vec3{1.0f, 0.34f, 0.2f};
	pointLightRender.source.attenConst = 1.0f;
	pointLightRender.source.attenLin = 0.09f;
	pointLightRender.source.attenQuad = 0.032f;

	pointLightRender.farPlane = 20.0f;
	pointLightRender.projMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, pointLightRender.farPlane);
	pointLightRender.viewMatrices[0] = glm::lookAt(pointLightRender.source.position,
												   pointLightRender.source.position + glm::vec3{1.0f, 0.0f, 0.0f}, 
												   {0.0f, -1.0f, 0.0f});
	pointLightRender.viewMatrices[1] = glm::lookAt(pointLightRender.source.position,
												   pointLightRender.source.position + glm::vec3{-1.0f, 0.0f, 0.0f}, 
												   {0.0f, -1.0f, 0.0f});
	pointLightRender.viewMatrices[2] = glm::lookAt(pointLightRender.source.position,
												   pointLightRender.source.position + glm::vec3{0.0f, 1.0f, 0.0f}, 
												   {0.0f, 0.0f, 1.0f});
	pointLightRender.viewMatrices[3] = glm::lookAt(pointLightRender.source.position,
												   pointLightRender.source.position + glm::vec3{0.0f, -1.0f, 0.0f}, 
												   {0.0f, 0.0f, -1.0f});
	pointLightRender.viewMatrices[4] = glm::lookAt(pointLightRender.source.position,
												   pointLightRender.source.position + glm::vec3{0.0f, 0.0f, 1.0f}, 
												   {0.0f, -1.0f, 0.0f});
	pointLightRender.viewMatrices[5] = glm::lookAt(pointLightRender.source.position,
												   pointLightRender.source.position + glm::vec3{0.0f, 0.0f, -1.0f}, 
												   {0.0f, -1.0f, 0.0f});
}

void Application::createLightShadowMaps()
{
	glGenTextures(1, &dirLightRender.shadowMap);
	glBindTexture(GL_TEXTURE_2D, dirLightRender.shadowMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT,
				 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glGenTextures(1, &pointLightRender.shadowCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, pointLightRender.shadowCubeMap);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for(int face = GL_TEXTURE_CUBE_MAP_POSITIVE_X; face <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; face++)
	{
		glTexImage2D(face, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 
					 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	}
}

size_t Application::createVBO()
{
	glm::vec2 cubeTexCoords[cube::NUM_VERTS] =
	{
		{0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f},
		{0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f},
		{0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f},
		{0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f},
		{0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f},
		{0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}
	};

	glm::vec2 floorTexCoords[xysquare::NUM_VERTS] =
	{
		{0.0f, 0.0f}, {0.0f, floorWidth / 4}, {floorWidth / 4, floorWidth / 4}, {floorWidth / 4, 0.0f}
	};

	using std::byte;
	
	std::vector<byte> vboVector;
	vboVector.insert(vboVector.end(), (byte*)cube::positions, (byte*)cube::positions + sizeof(cube::positions));
	vboVector.insert(vboVector.end(), (byte*)xysquare::positions, (byte*)xysquare::positions + sizeof(xysquare::positions));
	vboVector.insert(vboVector.end(), (byte*)cubeTexCoords, (byte*)cubeTexCoords + sizeof(cubeTexCoords));
	vboVector.insert(vboVector.end(), (byte*)floorTexCoords, (byte*)floorTexCoords + sizeof(floorTexCoords));
	vboVector.insert(vboVector.end(), (byte*)cube::normals, (byte*)cube::normals + sizeof(cube::normals));
	vboVector.insert(vboVector.end(), (byte*)xysquare::normals, (byte*)xysquare::normals + sizeof(xysquare::normals));

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
	glGenBuffers(1, &matsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, matsUBO);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera.projectionMatrix()));
	glUniformBlockBinding(shader, glGetUniformBlockIndex(shader, "mats"), matsUniformBinding);
	glUniformBlockBinding(skyboxShader, glGetUniformBlockIndex(skyboxShader, "mats"), matsUniformBinding);
	glBindBufferBase(GL_UNIFORM_BUFFER, matsUniformBinding, matsUBO);
}

void Application::createTextures()
{
	cubeDiffuseTexture = createObjectTexture("../res/container_diffuse.png", GL_SRGB);
	cubeSpecularTexture = createObjectTexture("../res/container_specular.png", GL_RGB);
	floorDiffuseTexture = createObjectTexture("../res/floor_diffuse.png", GL_SRGB);

	glBindTexture(GL_TEXTURE_2D, floorDiffuseTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	glGenTextures(1, &skybox);
	glActiveTexture(GL_TEXTURE0 + SKYBOX_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	texImageForSkybox(GL_TEXTURE_CUBE_MAP_POSITIVE_X, "../res/skybox_pos_x.png");
	texImageForSkybox(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, "../res/skybox_neg_x.png");
	texImageForSkybox(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, "../res/skybox_pos_y.png");
	texImageForSkybox(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, "../res/skybox_neg_y.png");
	texImageForSkybox(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, "../res/skybox_pos_z.png");
	texImageForSkybox(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, "../res/skybox_neg_z.png");

	glGenTextures(1, &floorSpecularTexture);
	glBindTexture(GL_TEXTURE_2D, floorSpecularTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	glGenTextures(1, &blackTexture);
	glBindTexture(GL_TEXTURE_2D, blackTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	glGenTextures(1, &lightCubeEmissiveTexture);
	glBindTexture(GL_TEXTURE_2D, lightCubeEmissiveTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

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

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, floorSpecularTexture, 0);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
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

	size_t vertexSize = sizeof(glm::vec3) + sizeof(glm::vec2) + sizeof(glm::vec3);

	glVertexAttribFormat(VERTEX_POS_ATTRIB_INDEX, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexAttribArray(VERTEX_POS_ATTRIB_INDEX);
	glBindVertexBuffer(OBJ_POS_BUFFER_BINDING, vbo, 0, sizeof(glm::vec3));

	glVertexAttribFormat(VERTEX_TEXCOORD_ATTRIB_INDEX, 2, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexAttribArray(VERTEX_TEXCOORD_ATTRIB_INDEX);
	glBindVertexBuffer(OBJ_TEXCOORD_BUFFER_BINDING, vbo, 
					   (size_t)(vboSize * sizeof(glm::vec3) / vertexSize), sizeof(glm::vec2));

	glVertexAttribFormat(VERTEX_NORMAL_ATTRIB_INDEX, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexAttribArray(VERTEX_NORMAL_ATTRIB_INDEX);
	glBindVertexBuffer(OBJ_NORMAL_BUFFER_BINDING, vbo, 
					   (size_t)(vboSize * (sizeof(glm::vec3) + sizeof(glm::vec2)) / vertexSize),
					   sizeof(glm::vec3));
	glVertexAttribBinding(VERTEX_NORMAL_ATTRIB_INDEX, OBJ_NORMAL_BUFFER_BINDING);

	glBindVertexBuffer(OBJ_TRANSMAT_BUFFER_BINDING, instanceVBO, 0, sizeof(glm::mat4));
	glVertexBindingDivisor(OBJ_TRANSMAT_BUFFER_BINDING, 1);
	for(int i = 0; i < 4; i++)
	{
		glVertexAttribFormat(i + VERTEX_TRANSMAT_ATTRIB_INDEX, 4, GL_FLOAT, GL_FALSE, i * sizeof(glm::vec4));
		glVertexAttribBinding(i + VERTEX_TRANSMAT_ATTRIB_INDEX, 4);
		glEnableVertexAttribArray(i + VERTEX_TRANSMAT_ATTRIB_INDEX);
	}

	glBindVertexBuffer(FB_POS_BUFFER_BINDING, fbQuadVBO, 0, sizeof(glm::vec3));
	glBindVertexBuffer(FB_TEXCOORD_BUFFER_BINDING, fbQuadVBO, 4 * sizeof(glm::vec3), sizeof(glm::vec2));
}

Application::Application() :
	SCREEN_WIDTH{1280}, SCREEN_HEIGHT{720},
	window{initialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Advanced OpenGL", 4, 6)},
	keys{window}, mouse{window},

	camera{glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f},
	CAMERA_SENSITIVITY{1.0f}, CAMERA_SPEED{15.0f},

	shader{createShaderProgram("../src/shaders/object_vs.glsl", "../src/shaders/object_fs.glsl")},
	fbShader{createShaderProgram("../src/shaders/framebuffer_vs.glsl", "../src/shaders/framebuffer_fs.glsl")},
	skyboxShader{createShaderProgram("../src/shaders/skybox_vs.glsl", "../src/shaders/skybox_fs.glsl")},
	dirShadowMapShader{createShaderProgram("../src/shaders/dir_shadow_map_vs.glsl", 
										   "../src/shaders/dir_shadow_map_fs.glsl")},
	omniShadowMapShader{createShaderProgram("../src/shaders/omni_shadow_map_vs.glsl",
											"../src/shaders/omni_shadow_map_gs.glsl",
										   "../src/shaders/omni_shadow_map_fs.glsl")},

	floorWidth{20.0f}, matsUniformBinding{0}, ambience{0.1f}, cubeShininess{128.0f}, floorShininess{32.0f}
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

	addTextureUniforms();
	addLightUniforms();

	size_t vboSize = createVBO();
	createEBO();

	initLightStructsAndMatrices();
	initObjectTransforms();
	createObjectTransformVBO();

	createMatrixUBO();
	createLightShadowMaps();
	createTextures();

	createPostProcessFBO();

	setVAOFormatsAndBuffers(vboSize);

	glUseProgram(shader);
	setDirLightUniform(uniforms, shader, "u_dirLight", dirLightRender.source); // Should be done in render loop if dynamic
	setPointLightUniform(uniforms, shader, "u_pointLight", pointLightRender.source); // Should be done in 
																					 // render loop if dynamic
	uniforms.setUniform(shader, "u_ambience", ambience);
	uniforms.setUniform(shader, "u_diffuse", DIFFUSE_TEXTURE_UNIT);   // These three can
	uniforms.setUniform(shader, "u_specular", SPECULAR_TEXTURE_UNIT); // have texture-arrays
	uniforms.setUniform(shader, "u_emissive", EMISSIVE_TEXTURE_UNIT); // of the same size
	uniforms.setUniform(shader, "u_dirLightShadowMap", SHADOW_MAP_TEXTURE_UNIT); // Texture-arrays probably
	uniforms.setUniform(shader, "u_pointLightShadowMap", SHADOW_MAP_TEXTURE_UNIT + 1); // Texture-arrays probably

	glUseProgram(skyboxShader);
	uniforms.setUniform(skyboxShader, "u_skybox", SKYBOX_TEXTURE_UNIT);

	glUseProgram(fbShader);
	uniforms.setUniform(fbShader, "fbTexture", FB_COLOR_TEXTURE_UNIT);
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
			camera.position += -camera.behind() * CAMERA_SPEED * deltaTime;
		}
		else if(keys.keyPressed("BACKWARD"))
		{
			camera.position += camera.behind() * CAMERA_SPEED * deltaTime;
		}
		if(keys.keyPressed("LEFT"))
		{
			camera.position += -camera.right() * CAMERA_SPEED * deltaTime;
		}
		else if(keys.keyPressed("RIGHT"))
		{
			camera.position += camera.right() * CAMERA_SPEED * deltaTime;
		}
		if(keys.keyPressed("UP"))
		{
			camera.position += camera.up() * CAMERA_SPEED * deltaTime;
		}
		else if(keys.keyPressed("DOWN"))
		{
			camera.position += -camera.up() * CAMERA_SPEED * deltaTime;
		}
		
		glm::vec3 camUp{0.0f, 1.0f, 0.0f};
		glm::vec3 camRight{camera.right()};
		
		float mouseMoveX = mouse.getMouseMovementX();
		if(mouseMoveX != 0)
		{
			camera.rotateGlobal(camUp, -mouseMoveX * CAMERA_SENSITIVITY * deltaTime);
		}

		float mouseMoveY = mouse.getMouseMovementY();
		if(mouseMoveY != 0)
		{
			camera.rotateGlobal(camRight, mouseMoveY * CAMERA_SENSITIVITY * deltaTime);
		}

		// Also set light source structs but that is constant for now.

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
	
		uniforms.setUniform(omniShadowMapShader, "u_lightSpaceProjMatrix", pointLightRender.projMatrix);
		for(int i = 0; i < 6; i++)
		{
			uniforms.setUniform(omniShadowMapShader, std::format("u_lightSpaceViewMatrices[{}]", i), 
								pointLightRender.viewMatrices[i]);
		}
		uniforms.setUniform(omniShadowMapShader, "u_farPlane", pointLightRender.farPlane);

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

		// Also set all light source struct uniforms but that is constant for now

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

		uniforms.setUniform(shader, "u_materialShininess", cubeShininess);
		glDrawElementsInstanced(GL_TRIANGLES, cube::NUM_INDICES, GL_UNSIGNED_INT, (const void*)0, NUM_CUBES);

		glActiveTexture(GL_TEXTURE0 + EMISSIVE_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, lightCubeEmissiveTexture);
		glActiveTexture(GL_TEXTURE0 + DIFFUSE_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, blackTexture);
		glActiveTexture(GL_TEXTURE0 + SPECULAR_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, blackTexture);

		glDrawElementsInstancedBaseInstance(GL_TRIANGLES, cube::NUM_INDICES, GL_UNSIGNED_INT, (const void*)0, 1, NUM_CUBES);

		glActiveTexture(GL_TEXTURE0 + EMISSIVE_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, blackTexture);
		glActiveTexture(GL_TEXTURE0 + DIFFUSE_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, floorDiffuseTexture);
		glActiveTexture(GL_TEXTURE0 + SPECULAR_TEXTURE_UNIT);
		glBindTexture(GL_TEXTURE_2D, floorSpecularTexture);

		uniforms.setUniform(shader, "u_materialShininess", floorShininess);
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