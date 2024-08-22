#include "models_and_objs.h"

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