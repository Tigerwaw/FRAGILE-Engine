#pragma once
#include <filesystem>
#include <string>
#include "Math/Vector.hpp"

class AppSettings
{
public:
    static void LoadSettings(const std::filesystem::path& aSettingsFilepath);
    static AppSettings& Get();
    static void Destroy();

    std::string title;
    std::filesystem::path contentRoot;
    Math::Vector2f resolution;
    Math::Vector2f windowSize;
    Math::Vector2f windowPos;
    bool isFullscreen = true;
    bool isBorderless = true;
    bool allowDropFiles = false;
    bool autoRegisterAssets = true;

private:
    AppSettings();
    ~AppSettings();
};