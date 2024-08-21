#include "models_and_objs.h"
#include "hash.h"
#include <cstring>

std::size_t std::hash<ModelInfo>::operator()(const ModelInfo& obj) const noexcept
{
    til::hasher hasher{};
    hasher.write(obj.indexCount);
    hasher.write(obj.eboOffset);
    hasher.write(obj.vboOffset);
    hasher.write(obj.diffuseMapID);
    hasher.write(obj.specularMapID);
    hasher.write(obj.emissiveMapID);
    hasher.write(obj.normalMapID);
    hasher.write(obj.dispMapID);
    return hasher.finalize();
}

bool operator==(const ModelInfo& obj1, const ModelInfo& obj2)
{
    return (obj1.indexCount == obj2.indexCount) &&
           (obj1.eboOffset == obj2.eboOffset) &&
           (obj1.vboOffset == obj2.vboOffset) &&
           (obj1.diffuseMapID == obj2.diffuseMapID) &&
           (obj1.specularMapID == obj2.specularMapID) &&
           (obj1.emissiveMapID == obj2.emissiveMapID) &&
           (obj1.normalMapID == obj2.normalMapID) &&
           (obj1.dispMapID == obj2.dispMapID);
}

InstanceInfo& ObjectSet::addObject(const ModelInfo& model, const InstanceInfo& object)
{
    if(!modelDrawCallIndexMap.contains(model))
    {
        modelDrawCallIndexMap[model] = drawCalls.size();
        DrawIndirect& newDrawCall = drawCalls.emplace_back(model.indexCount, 0, model.eboOffset, 
                                                            model.vboOffset, (unsigned int)instances.size());
    }

    DrawIndirect& drawCall = drawCalls[modelDrawCallIndexMap[model]];
    auto ret = instances.insert(instances.begin() + drawCall.instanceOffset, object);
    drawCall.instanceCount++;

    return *ret;
}

const DrawIndirect& ObjectSet::getDrawCall(const ModelInfo& model) const
{
    return drawCalls[modelDrawCallIndexMap.at(model)];
}

const std::vector<InstanceInfo>& ObjectSet::getObjectArray() const
{
    return instances;
}