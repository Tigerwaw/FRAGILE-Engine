#include "Enginepch.h"

#include "Model.h"
#include "Objects/Mesh.h"
#include "Objects/Material.h"
#include "ComponentSystem/GameObject.h"
#include "GraphicsEngine.h"
#include "AssetTypes/MaterialAsset.h"
#include "AssetTypes/MeshAsset.h"
#include "AssetManager.h"

Model::~Model()
{
    myMesh = nullptr;
    myMaterials.clear();
    mySlotToIndex.clear();
}

Model::Model()
{
    SetMaterialOnSlot(0, GraphicsEngine::Get().GetDefaultMaterial());
}

Model::Model(std::shared_ptr<Mesh> aMesh)
{
    myMesh = aMesh;
    for (auto& element : myMesh->GetSubmeshes())
    {
        SetMaterialOnSlot(element.MaterialIndex, GraphicsEngine::Get().GetDefaultMaterial());
    }
}

Model::Model(std::shared_ptr<Mesh> aMesh, std::shared_ptr<Material> aMaterial)
{
    myMesh = aMesh;
    for (auto& element : myMesh->GetSubmeshes())
    {
        SetMaterialOnSlot(element.MaterialIndex, aMaterial);
    }
}

void Model::Start()
{
}

void Model::Update()
{
}

void Model::SetMesh(std::shared_ptr<Mesh> aMesh)
{
    myMesh = aMesh;
}

void Model::SetMaterialOnSlot(unsigned aSlot, std::shared_ptr<Material> aMaterial)
{
    myMaterials.emplace_back(aMaterial);

    if (mySlotToIndex.find(aSlot) != mySlotToIndex.end())
    {
        myMaterials.erase(myMaterials.begin() + mySlotToIndex.at(aSlot));
        mySlotToIndex.erase(aSlot);
    }

    mySlotToIndex.emplace(aSlot, static_cast<unsigned>(myMaterials.size() - 1));
}

const Math::AABB3D<float> Model::GetBoundingBox() const
{
    if (!myMesh)
    {
        return Math::AABB3D<float>();
    }

    return myMesh->GetBoundingBox();
}

bool Model::Serialize(nl::json& outJsonObject)
{
    outJsonObject;
    return false;
}

bool Model::Deserialize(nl::json& aJsonObject)
{
    if (!aJsonObject.contains("Model")) return false;

    if (aJsonObject.contains("ShouldViewcull"))
    {
        SetViewcull(aJsonObject["ShouldViewcull"].get<bool>());
    }

    if (aJsonObject.contains("CastShadows"))
    {
        SetCastShadows(aJsonObject["CastShadows"].get<bool>());
    }

    std::string meshName = aJsonObject["Model"].get<std::string>();
    Utilities::ToLower(meshName);
    if (meshName == "sm_planeprimitive")
    {
        SetMesh(GraphicsEngine::Get().GetPlanePrimitive());
    }
    else if (meshName == "sm_cubeprimitive")
    {
        SetMesh(GraphicsEngine::Get().GetCubePrimitive());
    }
    else if (meshName == "sm_rampprimitive")
    {
        SetMesh(GraphicsEngine::Get().GetRampPrimitive());
    }
    else
    {
        SetMesh(AssetManager::Get().GetAsset<MeshAsset>(meshName)->mesh);
    }

    if (aJsonObject.contains("Materials"))
    {
        for (int i = 0; i < aJsonObject["Materials"].size(); i++)
        {
            SetMaterialOnSlot(i, AssetManager::Get().GetAsset<MaterialAsset>(aJsonObject["Materials"][i].get<std::string>())->material);
        }
    }

    return true;
}
