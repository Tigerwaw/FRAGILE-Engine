#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <Windowsx.h>
#include <wrl.h>

#ifndef _RETAIL
#include "Logger/Logger.h"
#define USE_PIX
#endif
#include "WinPixEventRuntime/pix3.h"

#include <memory>
#include <functional>
#include <algorithm>
#include <math.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
namespace nl = nlohmann;

#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <any>
#include <queue>
#include <mutex>
#include <shared_mutex>
#include <thread>

#include "Imgui/imgui.h"
#include "Imgui/backends/imgui_impl_win32.h"
#include "Imgui/backends/imgui_impl_dx11.h"
#include "Imgui/misc/cpp/imgui_stdlib.h"
#include "fmod/fmod.hpp"
#include "fmod/fmod_studio.hpp"
#include "fmod/fmod_common.h"

#ifdef _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(LogGameEngine, GameEngine, Verbose);
DECLARE_LOG_CATEGORY_WITH_NAME(LogApplication, Application, Verbose);
DECLARE_LOG_CATEGORY_WITH_NAME(LogAudioEngine, AudioEngine, Verbose);
DECLARE_LOG_CATEGORY_WITH_NAME(LogAudioPlayer, AudioInstance, Verbose);
DECLARE_LOG_CATEGORY_WITH_NAME(LogDebugDrawer, DebugDrawer, Verbose);
DECLARE_LOG_CATEGORY_WITH_NAME(LogScene, Scene, Verbose);
DECLARE_LOG_CATEGORY_WITH_NAME(LogSceneHandler, SceneHandler, Verbose);
DECLARE_LOG_CATEGORY_WITH_NAME(LogSceneLoader, SceneLoader, Verbose);
DECLARE_LOG_CATEGORY_WITH_NAME(LogInputHandler, InputHandler, Verbose);
DECLARE_LOG_CATEGORY_WITH_NAME(LogComponentSystem, ComponentSystem, Verbose);
#elif _RELEASE
DECLARE_LOG_CATEGORY_WITH_NAME(LogGameEngine, GameEngine, Warning);
DECLARE_LOG_CATEGORY_WITH_NAME(LogApplication, Application, Warning);
DECLARE_LOG_CATEGORY_WITH_NAME(LogAudioEngine, AudioEngine, Warning);
DECLARE_LOG_CATEGORY_WITH_NAME(LogAudioPlayer, AudioInstance, Warning);
DECLARE_LOG_CATEGORY_WITH_NAME(LogDebugDrawer, DebugDrawer, Warning);
DECLARE_LOG_CATEGORY_WITH_NAME(LogScene, Scene, Warning);
DECLARE_LOG_CATEGORY_WITH_NAME(LogSceneHandler, SceneHandler, Warning);
DECLARE_LOG_CATEGORY_WITH_NAME(LogSceneLoader, SceneLoader, Warning);
DECLARE_LOG_CATEGORY_WITH_NAME(LogInputHandler, InputHandler, Warning);
DECLARE_LOG_CATEGORY_WITH_NAME(LogComponentSystem, ComponentSystem, Warning);
#else
#define LOG(Category, Verbosity, Message, ...);
#endif