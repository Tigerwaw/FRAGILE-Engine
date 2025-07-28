#include "Enginepch.h"
#include "PSOAsset.h"
#include "GraphicsEngine.h"
#include "nlohmann/json.hpp"
namespace nl = nlohmann;
#include <fstream>
#include "AssetManager.h"
#include "ShaderAsset.h"
#include "TextureAsset.h"

bool PSOAsset::Load()
{
    std::ifstream streamPath(GetPath());
    nl::json data = nl::json();

    try
    {
        data = nl::json::parse(streamPath);
    }
    catch (nl::json::parse_error e)
    {
        return false;
    }
    streamPath.close();

    PSODescription psoDesc = {};

    psoDesc.name = GetName().string();

    if (data.contains("VertexType"))
    {
        psoDesc.vertexType = VertexTypeFromName(data["VertexType"].get<std::string>().c_str());
    }

    std::filesystem::path vsPath = "";
    std::filesystem::path gsPath = "";
    std::filesystem::path psPath = "";
    std::shared_ptr<ShaderAsset> vsShaderAsset;
    std::shared_ptr<ShaderAsset> gsShaderAsset;
    std::shared_ptr<ShaderAsset> psShaderAsset;

    auto& assetManager = AssetManager::Get();

    if (data.contains("VertexShader") && data["VertexShader"] != "")
    {
        vsShaderAsset = assetManager.GetAsset<ShaderAsset>(data["VertexShader"].get<std::string>());
        if (vsShaderAsset)
        {
            vsPath = vsShaderAsset->GetPath();
        }
    }

    if (data.contains("GeometryShader") && data["GeometryShader"] != "")
    {
        gsShaderAsset = assetManager.GetAsset<ShaderAsset>(data["GeometryShader"].get<std::string>());
        if (gsShaderAsset)
        {
            gsPath = gsShaderAsset->GetPath();
        }
    }

    if (data.contains("PixelShader") && data["PixelShader"] != "")
    {
        psShaderAsset = assetManager.GetAsset<ShaderAsset>(data["PixelShader"].get<std::string>());
        if (psShaderAsset)
        {
            psPath = psShaderAsset->GetPath();
        }
    }

#ifndef _RETAIL
    if (!GraphicsEngine::Get().GetResourceVendor().ValidateShaderCombination(vsPath, gsPath, psPath))
    {
        return false;
    }
#endif


    if (vsShaderAsset)
    {
        psoDesc.vsPath = vsPath.wstring();
        psoDesc.vsShader = vsShaderAsset->shader;
    }

    if (gsShaderAsset)
    {
        psoDesc.gsShader = gsShaderAsset->shader;
    }

    if (psShaderAsset)
    {
        psoDesc.psShader = psShaderAsset->shader;
    }

    if (data.contains("RasterizerDesc"))
    {
        if (data["RasterizerDesc"].contains("FillMode"))
        {
            psoDesc.fillMode = FillModeFromName(data["RasterizerDesc"]["FillMode"].get<std::string>().c_str());
        }

        if (data["RasterizerDesc"].contains("CullMode"))
        {
            psoDesc.cullMode = CullModeFromName(data["RasterizerDesc"]["CullMode"].get<std::string>().c_str());
        }

        if (data["RasterizerDesc"].contains("AntialiasedLine"))
        {
            psoDesc.antiAliasedLine = data["RasterizerDesc"]["AntialiasedLine"].get<bool>();
        }
    }

    if (data.contains("BlendStateDesc"))
    {
        if (data["BlendStateDesc"].contains("BlendMode"))
        {
            psoDesc.blendMode = BlendModeFromName(data["BlendStateDesc"]["BlendMode"].get<std::string>().c_str());
        }

        if (data["BlendStateDesc"].contains("AlphaToCoverage"))
        {
            psoDesc.alphaToCoverage = data["BlendStateDesc"]["AlphaToCoverage"].get<bool>();
        }

        if (data["BlendStateDesc"].contains("IndependentBlend"))
        {
            psoDesc.independentBlend = data["BlendStateDesc"]["IndependentBlend"].get<bool>();
        }
    }

    if (data.contains("UseReadOnlyDepthStencil"))
    {
        psoDesc.depthMode = DepthModeFromName(data["UseReadOnlyDepthStencil"].get<std::string>().c_str());
    }

    if (data.contains("Samplers"))
    {
        for (auto& sampler : data["Samplers"].items())
        {
            psoDesc.samplerList.emplace(sampler.value().get<unsigned>(), sampler.key());
        }
    }

    pso = std::make_shared<PipelineStateObject>();
    if (!GraphicsEngine::Get().GetResourceVendor().CreatePSO(*pso, psoDesc))
    {
        pso = nullptr;
        return false;
    }

    if (data.contains("Textures"))
    {
        unsigned textureIndex = GraphicsSettings::PSO_UTILITY_TEXTURES_START_SLOT;
        for (auto& texturePath : data["Textures"])
        {
            std::filesystem::path texPath = texturePath.get<std::string>();
            std::string texNameLower = Utilities::ToLowerCopy(texPath.filename().string());
            std::filesystem::path texName(texNameLower);
            if (auto texAsset = AssetManager::Get().GetAsset<TextureAsset>(texName))
            {
                pso->TextureResources[textureIndex] = texAsset->texture;
            }
            else
            {
                return false;
            }

            ++textureIndex;
        }
    }

    return true;
}

bool PSOAsset::Unload()
{
    pso = nullptr;
    return true;
}
