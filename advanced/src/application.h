#ifndef APPLICATION_H
#define APPLICATION_H

#include <utility/util.h>
#include <utility/keyboard_input.h>
#include <utility/mouse_input.h>
#include <utility/camera.h>
#include <utility/uniform_setter.h>
#include <utility/lighting.h>

struct ModelInfo
{
	unsigned int indexCount;
	size_t eboOffset;
	int vboOffset;
	unsigned int instanceCount;
	unsigned int instanceOffset;
	unsigned int diffuseMapID, specularMapID, emissiveMapID, normalMapID, dispMapID;
};

class Application
{
private :
	void addLightUniforms();
	void initLightStructsAndMatrices();
	void createLightShadowMaps();
	void addTextureUniforms();
	size_t createVBO();
	void createEBO();
	void initObjectTransforms();
	void createObjectTransformVBO();
	void setModelInfos();
	void createMatrixUBO();
	void createTextureMaps();
	void createPostProcessFBO();
	void setVAOFormatsAndBuffers(size_t vboSize);
	void setUniforms();

	static constexpr int FB_COLOR_TEXTURE_UNIT = 0,
		SKYBOX_TEXTURE_UNIT = 1,
		DIFFUSE_TEXTURE_UNIT = 2,
		SPECULAR_TEXTURE_UNIT = 3,
		EMISSIVE_TEXTURE_UNIT = 4,
		NORMAL_TEXTURE_UNIT = 5,
		DISPLACEMENT_TEXTURE_UNIT = 6,
		SHADOW_MAP_TEXTURE_UNIT = 7;

	static constexpr int VERTEX_POS_ATTRIB_INDEX = 0,
		VERTEX_TEXCOORD_ATTRIB_INDEX = 1,
		VERTEX_TRANSMAT_ATTRIB_INDEX = 2,
		VERTEX_NORMAL_ATTRIB_INDEX = 6,
		VERTEX_TANGENT_ATTRIB_INDEX = 7;

	static constexpr int VERTEX_POS_SIZE = sizeof(glm::vec3),
		VERTEX_TEXCOORD_SIZE = sizeof(glm::vec2),
		VERTEX_NORMAL_SIZE = sizeof(glm::vec3),
		VERTEX_TANGENT_SIZE = sizeof(glm::vec3),
		VERTEX_TOTAL_SIZE = VERTEX_POS_SIZE + VERTEX_TEXCOORD_SIZE + VERTEX_NORMAL_SIZE + VERTEX_TANGENT_SIZE;

	static constexpr int OBJ_POS_BUFFER_BINDING = 0,
		OBJ_TEXCOORD_BUFFER_BINDING = 1,
		FB_POS_BUFFER_BINDING = 2,
		FB_TEXCOORD_BUFFER_BINDING = 3,
		OBJ_TRANSMAT_BUFFER_BINDING = 4,
		OBJ_NORMAL_BUFFER_BINDING = 5,
		OBJ_TANGENT_BUFFER_BINDING = 6;

	const int SCREEN_WIDTH, SCREEN_HEIGHT;
	GLFWwindow* window;
	KeyboardInput keys;
	MouseInput mouse;
	float prevTime;

	Camera camera;
	float camSensitivity, camSpeed;

	float pointLightBrightnessSpeed, pointLightMoveSpeed;
	glm::vec3 maxBrightDiffuseColor, maxBrightSpecularColor;

	unsigned int vao, vbo, ebo;
	unsigned int instanceVBO;
	unsigned int matsUBO;
	unsigned int matsUniformBinding;

	ModelInfo woodCubeModelInfo, lightCubeModelInfo, floorModelInfo;

	unsigned int blackTexture, whiteTexture, defaultNormalTexture;
	unsigned int skybox;
	unsigned int textureWriteFBO;
	float cubeShininess, floorShininess, floorHeightScale, lightCubeEmissiveStrength;
	int numDispLayers;

	DirectionalLightRender dirLightRender;
	PointLightRender pointLightRender;
	float ambience;
	static constexpr int SHADOW_MAP_WIDTH = 1024,
						 SHADOW_MAP_HEIGHT = 1024;

	unsigned int shader;
	unsigned int fbShader;
	unsigned int skyboxShader;
	unsigned int dirShadowMapShader, omniShadowMapShader;

	unsigned int fbo;
	unsigned int fbColorTexture;
	unsigned int rbo;
	unsigned int fbQuadVBO;
	unsigned int fbQuadEBO;

	static constexpr int NUM_CUBES = 4;
	glm::mat4 cubeTransformMats[NUM_CUBES];
	glm::mat4 floorTransformMat;
	float floorWidth;
	glm::mat4 lightCubeTransformMat;

	UniformSetter uniforms;

public :
	Application();
	~Application();

	void run();
};

#endif