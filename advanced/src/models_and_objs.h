#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>

struct ModelInfo
{
	unsigned int indexCount;
	unsigned int eboOffset;
	int vboOffset;
};

struct DrawIndirect
{
	unsigned int indexCount;
	unsigned int instanceCount;
	unsigned int eboOffset;
	int vboOffset;
	unsigned int objectOffset;
};

class ObjectSet
{
private :
    typedef size_t DrawCallIndex;

    struct ObjectInfo
    {
        glm::mat4 transform;
    };

	std::unordered_map<ModelInfo, DrawCallIndex> modelDrawCallIndexMap;
    std::vector<ObjectInfo> objects;
    std::vector<DrawIndirect> drawCalls;

public :
    void addObject(const ModelInfo& model, const glm::mat4& transform)
    {
        if(!modelDrawCallIndexMap.contains(model))
        {
            modelDrawCallIndexMap[model] = drawCalls.size();
            DrawIndirect& newDrawCall = drawCalls.emplace_back();
            newDrawCall = {model.indexCount, 0, model.eboOffset, model.vboOffset, (unsigned int)objects.size()};
        }

        drawCalls[modelObjectMap[model].drawCallIndex].instanceCount++;
    }
};