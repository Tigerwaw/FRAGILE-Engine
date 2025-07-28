#include "Enginepch.h"
#include "MaterialAsset.h"
#include "nlohmann/json.hpp"
namespace nl = nlohmann;
#include <fstream>
#include "AssetManager.h"
#include "PSOAsset.h"
#include "TextureAsset.h"
#include "GraphicsEngine.h"

bool MaterialAsset::Load()
{
    std::ifstream streamPath(GetPath());
    nl::json data = nl::json();

    try
    {
        data = nl::json::parse(streamPath);
    }
    catch (nl::json::parse_error)
    {
        return false;
    }
    streamPath.close();

    Material mat;

    if (data.contains("PSO"))
    {
        std::string psoName = data["PSO"].get<std::string>();
        if (GraphicsEngine::Get().HasPSO(psoName.c_str()))
        {
            mat.SetPSO(GraphicsEngine::Get().GetPSO(psoName.c_str()));
        }
        else
        {
            mat.SetPSO(AssetManager::Get().GetAsset<PSOAsset>(psoName)->pso);
        }
    }

    if (data.contains("AlbedoTint"))
    {
        mat.MaterialSettings().albedoTint = { data["AlbedoTint"]["R"].get<float>(),
                                              data["AlbedoTint"]["G"].get<float>(),
                                              data["AlbedoTint"]["B"].get<float>(),
                                              data["AlbedoTint"]["A"].get<float>() };
    }

    if (data.contains("UVScale"))
    {
        mat.MaterialSettings().uvScale = { data["UVScale"]["X"].get<float>(),
                                           data["UVScale"]["Y"].get<float>() };
    }

    if (data.contains("UVOffset"))
    {
        mat.MaterialSettings().uvOffset = { data["UVOffset"]["X"].get<float>(),
                                            data["UVOffset"]["Y"].get<float>() };
    }

    if (data.contains("EmissiveStrength"))
    {
        mat.MaterialSettings().emissiveStrength = data["EmissiveStrength"].get<float>();
    }

    if (data.contains("Textures"))
    {
        unsigned textureIndex = 0;
        for (auto& texturePath : data["Textures"])
        {
            std::filesystem::path texPath = texturePath.get<std::string>();
            std::string texNameLower = Utilities::ToLowerCopy(texPath.filename().string());
            std::filesystem::path texName(texNameLower);
            if (auto texAsset = AssetManager::Get().GetAsset<TextureAsset>(texName))
            {
                mat.SetTextureOnSlot(textureIndex, texAsset->texture);
            }
            else
            {
                return false;
            }

            ++textureIndex;
        }
    }

    material = std::make_shared<Material>(std::move(mat));
    return true;
}

bool MaterialAsset::Unload()
{
    material = nullptr;
    return true;
}
