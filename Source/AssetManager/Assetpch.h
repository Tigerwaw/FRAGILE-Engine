#pragma once
#include "nlohmann/json.hpp"
namespace nl = nlohmann;
#include "TGAFBX/TGAFbx.h"

#include <d3d11.h>
#include <wrl.h>

#ifndef _RETAIL
#define USE_PIX
#endif
#include "WinPixEventRuntime/pix3.h"

#include <memory>
#include <mutex>
#include <cstdint>
#include <cstring>
#include <functional>
#include <stdexcept>
#include <filesystem>
#include <iostream>
#include <fstream>

#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <any>
#include <future>
#include <thread>
#include <chrono>

#include "Logger/Logger.h"

#ifdef _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(LogAssetManager, AssetManager, Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(LogAssetManager, AssetManager, Warning);
#endif