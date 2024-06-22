#ifndef APPLICATION_H
#define APPLICATION_H

#include <utility/util.h>
#include <utility/keyboard_input.h>
#include <utility/mouse_input.h>
#include <utility/camera.h>
#include <utility/transform.h>
#include <utility/uniform_setter.h>

struct PointLightRender
{
	unsigned int shadowCubeMap;
	glm::mat4 projMatrix;
	glm::mat4 viewMatrices[6];
	float farPlane;
	PointLight source;
};

struct DirectionalLightRender
{
	unsigned int shadowMap;
	glm::mat4 matrix;
	DirectionalLight source;
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
	void createMatrixUBO();
	void createTextures();

	void createPostProcessFBO();

	void setVAOFormatsAndBuffers(size_t vboSize);

	void setUniforms();

	static constexpr int FB_COLOR_TEXTURE_UNIT = 0,
		SKYBOX_TEXTURE_UNIT = 1,
		DIFFUSE_TEXTURE_UNIT = 2,
		SPECULAR_TEXTURE_UNIT = 3,
		EMISSIVE_TEXTURE_UNIT = 4,
		NORMAL_TEXTURE_UNIT = 5,
		SHADOW_MAP_TEXTURE_UNIT = 6;

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
	const float CAMERA_SENSITIVITY, CAMERA_SPEED;

	unsigned int vao, vbo, ebo;
	unsigned int instanceVBO;
	unsigned int matsUBO;
	const unsigned int matsUniformBinding;

	unsigned int blackTexture, whiteTexture, defaultNormalTexture;
	unsigned int skybox;
	unsigned int textureWriteFBO;
	unsigned int cubeDiffuseTexture, floorDiffuseTexture;
	unsigned int cubeSpecularTexture, floorSpecularTexture;
	unsigned int lightCubeEmissiveTexture;
	unsigned int floorNormalTexture;
	float cubeShininess, floorShininess;

	DirectionalLightRender dirLightRender;
	PointLightRender pointLightRender;
	const float ambience;
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
	const float floorWidth;
	glm::mat4 lightCubeTransformMat;

	UniformSetter uniforms;

public :
	Application();
	~Application();

	void run();
};

#endif