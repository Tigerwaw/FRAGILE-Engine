#pragma once
#include "ComponentSystem/Component.h"
#include "Objects/DynamicVertexBuffer.h"

class Model;

class InstancedModel : public Component
{
public:
    InstancedModel(unsigned aMaxInstances = 100);

    void Start() override;
    void Update() override;

    void SetMesh(std::shared_ptr<Mesh> aMesh);
    std::shared_ptr<Mesh> GetMesh() { return myMesh; }
    unsigned GetMeshCount() { return static_cast<unsigned>(myMeshTransforms.size()); }
    DynamicVertexBuffer& GetInstanceBuffer() { return myMeshTransformBuffer; }

    void AddInstance(const Math::Matrix4x4f& aTransform);
    void AddInstanceNoBufferUpdate(const Math::Matrix4x4f& aTransform);
    void UpdateInstanceBuffer();

    void SetMaterialOnSlot(unsigned aSlot, std::shared_ptr<Material> aMaterial);
    std::shared_ptr<Material>& GetMaterialOnSlot(unsigned aSlot) { return myMaterials[mySlotToIndex[aSlot]]; }
    const std::vector<std::shared_ptr<Material>>& GetMaterials() { return myMaterials; }
    const Math::AABB3D<float> GetBoundingBox() const { return myBoundingBox; }

    void SetViewcull(bool aShouldViewcull) { myShouldViewcull = aShouldViewcull; }
    const bool GetShouldViewcull() const { return myShouldViewcull; }
    void SetCastShadows(bool aCastShadows) { myCastShadows = aCastShadows; }
    const bool GetCastShadows() const { return myCastShadows; }

    bool Serialize(nl::json& outJsonObject) override;
    bool Deserialize(nl::json& aJsonObject) override;
    
private:
    std::shared_ptr<Mesh> myMesh = nullptr;
    std::vector<std::shared_ptr<Material>> myMaterials;
    std::unordered_map<unsigned, unsigned> mySlotToIndex;
    bool myShouldViewcull = true;
    bool myCastShadows = true;

    int myMaxInstances;
    DynamicVertexBuffer myMeshTransformBuffer;
    std::vector<Math::Matrix4x4f> myMeshTransforms;

    Math::AABB3D<float> myBoundingBox;
};