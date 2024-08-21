#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>

struct ModelInfo
{
	unsigned int indexCount;
	unsigned int eboOffset;
	int vboOffset;
    unsigned int diffuseMapID, specularMapID, emissiveMapID, normalMapID, dispMapID;
};

template<>
struct std::hash<ModelInfo> 
{
	std::size_t operator()(const ModelInfo& obj) const noexcept;
};

bool operator==(const ModelInfo& obj1, const ModelInfo& obj2);

struct InstanceInfo
{
    glm::mat4 transform;
};

struct DrawIndirect
{
	unsigned int indexCount;
	unsigned int instanceCount;
	unsigned int eboOffset;
	int vboOffset;
	unsigned int instanceOffset;
};

class ObjectSet
{
private :
    typedef size_t DrawCallIndex;

	std::unordered_map<ModelInfo, DrawCallIndex> modelDrawCallIndexMap;
    std::vector<InstanceInfo> instances;
    std::vector<DrawIndirect> drawCalls;

public :
    InstanceInfo& addObject(const ModelInfo& model, const InstanceInfo& object);
    const DrawIndirect& getDrawCall(const ModelInfo& model) const;
    const std::vector<InstanceInfo>& getObjectArray() const;
};