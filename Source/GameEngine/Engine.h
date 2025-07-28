#pragma once
#include "Math/Vector.hpp"

class SceneHandler;
class DebugDrawer;
class AudioEngine;
class InputHandler;
class Timer;
class ImGuiHandler;
class Application;
class WindowsEventHandler;
class Window;

class Engine
{
public:
    static Engine& Get();
    static void Initialize();
    static void Shutdown();

    void Prepare();
    void Update();
    void Render();

    Timer& GetTimer() { return *myTimer; }
    InputHandler& GetInputHandler() { return *myInputHandler; }
    SceneHandler& GetSceneHandler() { return *mySceneHandler; }
    DebugDrawer& GetDebugDrawer() { return *myDebugDrawer; }
    AudioEngine& GetAudioEngine() { return *myAudioEngine; }
    ImGuiHandler& GetImGuiHandler() { return *myImGuiHandler; }
    WindowsEventHandler& GetWindowsEventHandler() { return *myEventHandler; }
    Window& GetApplicationWindow() { return *myWindow; }

    const std::filesystem::path& GetContentRootPath();
    const std::string& GetApplicationTitle();

    void SetResolution(float aWidth, float aHeight);
    void SetWindowSize(float aWidth, float aHeight);
    void SetWindowPos(float aTop, float aLeft);
    void ToggleFullscreen(bool aIsFullscreen);
    Math::Vector2f GetResolution() const { return myResolution; }
    Math::Vector2f GetWindowSize() const { return myWindowSize; }
    Math::Vector2f GetWindowPos() const { return myWindowPos; }
    bool GetIsFullscreen() const { return myIsFullscreen; }
    bool GetIsBorderless() const { return myIsBorderless; }
    bool GetAllowDropFiles() const { return myAllowDropFiles; }
    bool GetAutoRegisterAssets() const { return myAutoRegisterAssets; }

    bool DrawBoundingBoxes = false;
    bool DrawCameraFrustums = false;
    bool DrawColliders = false;
    bool UseViewCulling = true;
    bool RecalculateShadowFrustum = true;
private:
    Engine();
    ~Engine();
    Engine(Engine const&) = delete;
    void operator=(Engine const&) = delete;
    void RegisterDefaultAssetTypes();
    
    std::unique_ptr<Window> myWindow;
    std::unique_ptr<WindowsEventHandler> myEventHandler;
    std::unique_ptr<Timer> myTimer;
    std::unique_ptr<InputHandler> myInputHandler;
    std::unique_ptr<SceneHandler> mySceneHandler;
    std::unique_ptr<DebugDrawer> myDebugDrawer;
    std::unique_ptr<AudioEngine> myAudioEngine;
    std::unique_ptr<ImGuiHandler> myImGuiHandler;

    std::string myTitle;
    std::filesystem::path myContentRoot;
    Math::Vector2f myResolution;
    Math::Vector2f myWindowSize;
    Math::Vector2f myWindowPos;
    bool myIsFullscreen = true;
    bool myIsBorderless = true;
    bool myAllowDropFiles = false;
    bool myAutoRegisterAssets = true;
};