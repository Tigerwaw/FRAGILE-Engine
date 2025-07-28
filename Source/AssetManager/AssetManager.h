#pragma once
#include <cassert>
#include <chrono>
#include "Asset.h"
#include "CommonUtilities/StringUtilities.hpp"
#include <functional>

class AssetManager
{
public:
	static AssetManager& Get()
	{
		static AssetManager myInstance;
		return myInstance;
	}

	bool Initialize(const std::filesystem::path& aContentRootPath, bool aAutoRegisterAllAssetsInRoot = true);
	std::filesystem::path& GetContentRoot() { return myContentRoot; }

	void Update();

	void RegisterAssetType(const std::string& aFileExtension, std::function<bool(const std::string&, const std::filesystem::path&)> aRegisterFunction);

	template<typename T>  requires std::is_base_of_v<Asset, T>
	bool RegisterAsset(const std::filesystem::path& aPath);

	template<typename T> requires std::is_base_of_v<Asset, T>
	std::shared_ptr<T> GetAsset(const std::filesystem::path& aPath);

	bool DeregisterAsset(const std::filesystem::path& aPath);
	bool IsAssetRegistered(const std::filesystem::path& aPath);
private:
	AssetManager();
	~AssetManager();
	bool ValidateAssetPath(const std::filesystem::path& aPath);
	bool FilenameHasPrefix(const std::filesystem::path& aPath, const char* aPrefixCompare) const;
	bool FilenameHasExtension(const std::filesystem::path& aPath, const char* aExtensionCompare) const;

	void RegisterAllAssetsInDirectory();
	std::shared_ptr<Asset> GetAssetBase(const std::filesystem::path& aPath);
	bool LoadAsset(const std::filesystem::path& aPath);
	bool UnloadAsset(const std::filesystem::path& aPath);
	
	void LogAssetLoadError(const std::filesystem::path& aPath);
	
	std::filesystem::path myContentRoot;
	std::unordered_map<std::filesystem::path, std::shared_ptr<Asset>> myAssets;
	std::unordered_map<std::string, std::function<bool(const std::string&, const std::filesystem::path&)>> myFileExtensionToRegisterFunc;

	void ClearUnusedAssets();
	bool myShouldAutoUnloadUnusedAssets = true;
	std::chrono::system_clock::time_point myLastClearUnusedAssetsTimepoint;
	float myTimeBetweenUnusedAssetClears = 10.0f;
};

inline void AssetManager::RegisterAssetType(const std::string& aFileExtension, std::function<bool(const std::string&, const std::filesystem::path&)> aRegisterFunction)
{
	myFileExtensionToRegisterFunc[aFileExtension] = aRegisterFunction;
}

template<typename T>  requires std::is_base_of_v<Asset, T>
bool AssetManager::RegisterAsset(const std::filesystem::path& aPath)
{
	std::shared_ptr<T> asset = std::make_shared<T>();
	asset->myPath = aPath;
	asset->myName = Utilities::ToLowerCopy(aPath.filename().string());
	asset->myIsLoaded = false;

	myAssets.emplace(asset->myName, asset);
	return true;
}

template<typename T> requires std::is_base_of_v<Asset, T>
inline std::shared_ptr<T> AssetManager::GetAsset(const std::filesystem::path& aPath)
{
	std::shared_ptr<T> asset = nullptr;
	
	if (IsAssetRegistered(aPath))
	{
		std::shared_ptr<Asset> assetBase = GetAssetBase(aPath);
		if (assetBase->IsLoaded())
		{
			asset = std::dynamic_pointer_cast<T>(assetBase);
			return asset;
		}
		else
		{
			LoadAsset(aPath);
			asset = std::dynamic_pointer_cast<T>(assetBase);
			return asset;
		}
	}
	else
	{
		LogAssetLoadError(aPath);
	}

	return nullptr;
}