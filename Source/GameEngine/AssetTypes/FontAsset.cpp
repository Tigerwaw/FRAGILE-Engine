#include "Enginepch.h"
#include "FontAsset.h"
#include "TextureAsset.h"
#include "AssetManager.h"
#include <filesystem>
#include <fstream>
#include "nlohmann/json.hpp"
namespace nl = nlohmann;

bool FontAsset::Load()
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

    Font newFont;

    std::filesystem::path textureName = GetName().stem().concat("_F.dds");
    if (!AssetManager::Get().IsAssetRegistered(textureName))
    {
        return false;
    }

    newFont.Texture = AssetManager::Get().GetAsset<TextureAsset>(textureName.filename())->texture;

    newFont.Atlas.Size = data["atlas"]["size"];
    newFont.Atlas.Width = data["atlas"]["width"];
    newFont.Atlas.Height = data["atlas"]["height"];
    newFont.Atlas.EmSize = data["metrics"]["emSize"];
    newFont.Atlas.LineHeight = data["metrics"]["lineHeight"];
    newFont.Atlas.Ascender = data["metrics"]["ascender"];
    newFont.Atlas.Descender = data["metrics"]["descender"];

    size_t glyphCount = data["glyphs"].size();
    for (int i = 0; i < glyphCount; i++)
    {
        Font::Glyph newGlyph;
        nl::json glyph = data["glyphs"][i];
        unsigned unicode = glyph.value("unicode", 0);
        float advance = glyph.value("advance", -1.0f);

        Math::Vector4f planeBounds;
        if (glyph.contains("planeBounds"))
        {
            planeBounds.x = glyph["planeBounds"]["left"].get<float>();
            planeBounds.y = glyph["planeBounds"]["bottom"].get<float>();
            planeBounds.z = glyph["planeBounds"]["right"].get<float>();
            planeBounds.w = glyph["planeBounds"]["top"].get<float>();
        }

        Math::Vector4f uvBounds;
        float atlasWidth = static_cast<float>(newFont.Atlas.Width);
        float atlasHeight = static_cast<float>(newFont.Atlas.Height);
        if (glyph.contains("atlasBounds"))
        {
            float UVStartX = glyph["atlasBounds"]["left"].get<float>() / atlasWidth;
            float UVStartY = glyph["atlasBounds"]["bottom"].get<float>() / atlasHeight;
            float UVEndX = glyph["atlasBounds"]["right"].get<float>() / atlasWidth;
            float UVEndY = glyph["atlasBounds"]["top"].get<float>() / atlasHeight;

            uvBounds = { UVStartX, UVStartY, UVEndX, UVEndY };
        }

        newGlyph.Character = static_cast<char>(unicode);
        newGlyph.Advance = advance;
        newGlyph.PlaneBounds = planeBounds;
        newGlyph.UVBounds = uvBounds;
        newFont.Glyphs.emplace(unicode, newGlyph);
    }

    font = std::make_shared<Font>(std::move(newFont));
    return true;
}

bool FontAsset::Unload()
{
    font = nullptr;
    return true;
}
