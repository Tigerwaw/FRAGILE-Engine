#include "Enginepch.h"

#include "AmbientLight.h"
#include "GraphicsEngine.h"
#include "Objects/Texture.h"
#include "AssetTypes/TextureAsset.h"
#include "AssetTypes/MeshAsset.h"
#include "AssetManager.h"

AmbientLight::AmbientLight(std::shared_ptr<Texture> aCubemap, Math::Vector3f aColor, float aIntensity)
{
	myCubemap = aCubemap;
	myColor = aColor;
	myIntensity = aIntensity;
}

void AmbientLight::Start()
{
	mySkyboxMesh = AssetManager::Get().GetAsset<MeshAsset>("sm_sphere.fbx")->mesh;
}

void AmbientLight::Update()
{
}

void AmbientLight::SetColor(Math::Vector3f aColor)
{
	myColor = aColor;
}

void AmbientLight::SetIntensity(float aIntensity)
{
	myIntensity = aIntensity;
}

void AmbientLight::SetCubemap(std::shared_ptr<Texture> aCubemap)
{
	myCubemap = aCubemap;
}

std::shared_ptr<Texture> AmbientLight::GetCubemap() const
{
	return myCubemap;
}

std::shared_ptr<Mesh> AmbientLight::GetSkyboxMesh() const
{
	return mySkyboxMesh;
}

bool AmbientLight::Serialize(nl::json& outJsonObject)
{
	outJsonObject;
	return false;
}

bool AmbientLight::Deserialize(nl::json& aJsonObject)
{
	if (aJsonObject.contains("Intensity"))
	{
		SetIntensity(aJsonObject["Intensity"].get<float>());
	}

	if (aJsonObject.contains("Cubemap"))
	{
		SetCubemap(AssetManager::Get().GetAsset<TextureAsset>(aJsonObject["Cubemap"].get<std::string>())->texture);
	}

	return true;
}
