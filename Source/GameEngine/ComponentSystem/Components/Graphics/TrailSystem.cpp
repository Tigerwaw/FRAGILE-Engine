#include "Enginepch.h"
#include "TrailSystem.h"
#include <d3d11.h>

#include "Engine.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"
#include "Time/Timer.h"
#include "CommonUtilities/SerializationUtils.hpp"
#include "AssetManager.h"
#include "AssetTypes/MaterialAsset.h"

TrailSystem::~TrailSystem()
{
}

void TrailSystem::Start()
{
}

void TrailSystem::Update()
{
	float dt = Engine::Get().GetTimer().GetDeltaTime();

	for (auto& emitter : myEmitters)
	{
		emitter.Update(gameObject->GetComponent<Transform>()->GetTranslation(true), dt);
	}

	UpdateBoundingBox();
}

void TrailSystem::ResetEmitters()
{
	for (auto& emitter : myEmitters)
	{
		emitter.ResetTrail();
	}

	UpdateBoundingBox();
}

TrailEmitter& TrailSystem::AddEmitter(const TrailEmitterSettings& aSettings)
{
	TrailEmitter& emitter = myEmitters.emplace_back(TrailEmitter());
	emitter.mySettings = aSettings;
	emitter.InitInternal();
	myBoundingBox = emitter.GetBoundingBox();
	return emitter;
}

bool TrailSystem::Serialize(nl::json&)
{
    return false;
}

bool TrailSystem::Deserialize(nl::json& aJsonObject)
{
	if (aJsonObject.contains("Emitters"))
	{
		for (auto& emitter : aJsonObject["Emitters"])
		{
			TrailEmitterSettings teSettings;

			if (emitter.contains("Vertices"))
			{
				teSettings.Vertices = emitter["Vertices"].get<unsigned>();
			}

			if (emitter.contains("Length"))
			{
				teSettings.Length = emitter["Length"].get<unsigned>();
			}

			if (emitter.contains("Width"))
			{
				for (auto& key : emitter["Width"])
				{
					teSettings.Width.AddKey(key["Time"].get<float>(), key["Value"].get<float>());
				}
			}

			if (emitter.contains("Color"))
			{
				for (auto& key : emitter["Color"])
				{
					teSettings.Color.AddKey(key["Time"].get<float>(), Utilities::DeserializeVector4<float>(key["Value"]));
				}
			}

			if (emitter.contains("ChannelMask"))
			{
				teSettings.ChannelMask = Utilities::DeserializeVector4<float>(emitter["ChannelMask"]);
			}

			TrailEmitter& te = AddEmitter(teSettings);

			if (emitter.contains("Material"))
			{
				te.SetMaterial(AssetManager::Get().GetAsset<MaterialAsset>(emitter["Material"].get<std::string>())->material);
			}

			te.InitInternal();
		}
	}

	return true;
}

void TrailSystem::UpdateBoundingBox()
{
	myBoundingBox = myEmitters[0].GetBoundingBox();

	for (auto& emitter : myEmitters)
	{
		Math::Vector3f bbMin = myBoundingBox.GetMin();
		Math::Vector3f bbMax = myBoundingBox.GetMax();

		Math::Vector3f bbEmitterMin = emitter.GetBoundingBox().GetMin();
		Math::Vector3f bbEmitterMax = emitter.GetBoundingBox().GetMax();

		bbMin.x = std::fminf(bbEmitterMin.x, bbMin.x);
		bbMax.x = std::fmaxf(bbEmitterMax.x, bbMax.x);
		bbMin.y = std::fminf(bbEmitterMin.y, bbMin.y);
		bbMax.y = std::fmaxf(bbEmitterMax.y, bbMax.y);
		bbMin.z = std::fminf(bbEmitterMin.z, bbMin.z);
		bbMax.z = std::fmaxf(bbEmitterMax.z, bbMax.z);

		myBoundingBox.InitWithMinAndMax(bbMin, bbMax);
	}
}