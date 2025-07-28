#include "Enginepch.h"
#include "ParticleSystem.h"
#include <d3d11.h>

#include "Engine.h"
#include "Time/Timer.h"
#include "CommonUtilities/SerializationUtils.hpp"
#include "AssetManager.h"

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Start()
{

}

void ParticleSystem::Update()
{
	float dt = Engine::Get().GetTimer().GetDeltaTime();

	for (auto& emitter : myEmitters)
	{
		emitter.Update(dt);
	}
}

ParticleEmitter& ParticleSystem::AddEmitter(const ParticleEmitterSettings& aSettings)
{
	ParticleEmitter& emitter = myEmitters.emplace_back(ParticleEmitter());
	emitter.mySettings = aSettings;
	emitter.InitInternal();
	return emitter;
}

bool ParticleSystem::Serialize(nl::json& outJsonObject)
{
	outJsonObject;
	return false;
}

bool ParticleSystem::Deserialize(nl::json& aJsonObject)
{
	if (aJsonObject.contains("Emitters"))
	{
		for (auto& emitter : aJsonObject["Emitters"])
		{
			ParticleEmitterSettings peSettings;

			if (emitter.contains("EmitterSize"))
			{
				peSettings.EmitterSize = Utilities::DeserializeVector3<float>(emitter["EmitterSize"]);
			}

			if (emitter.contains("SpawnRate"))
			{
				peSettings.SpawnRate = emitter["SpawnRate"].get<float>();
			}

			if (emitter.contains("Lifetime"))
			{
				peSettings.Lifetime = emitter["Lifetime"].get<float>();
			}

			if (emitter.contains("Size"))
			{
				for (auto& key : emitter["Size"])
				{
					peSettings.Size.AddKey(key["Time"].get<float>(), Utilities::DeserializeVector2<float>(key["Value"]));
				}
			}

			if (emitter.contains("Angle"))
			{
				for (auto& key : emitter["Angle"])
				{
					peSettings.Angle.AddKey(key["Time"].get<float>(), key["Value"].get<float>());
				}
			}

			if (emitter.contains("Velocity"))
			{
				if (emitter["Velocity"].contains("Min") && emitter["Velocity"].contains("Max"))
				{
					peSettings.RandomVelocity = true;
					peSettings.VelocityRangeMin = Utilities::DeserializeVector3<float>(emitter["Velocity"]["Min"]);
					peSettings.VelocityRangeMax = Utilities::DeserializeVector3<float>(emitter["Velocity"]["Max"]);
				}
				else
				{
					peSettings.Velocity = Utilities::DeserializeVector3<float>(emitter["Velocity"]);
				}
			}

			if (emitter.contains("GravityScale"))
			{
				peSettings.GravityScale = emitter["GravityScale"].get<float>();
			}

			if (emitter.contains("Color"))
			{
				for (auto& key : emitter["Color"])
				{
					peSettings.Color.AddKey(key["Time"].get<float>(), Utilities::DeserializeVector4<float>(key["Value"]));
				}
			}

			if (emitter.contains("ChannelMask"))
			{
				peSettings.ChannelMask = Utilities::DeserializeVector4<float>(emitter["ChannelMask"]);
			}

			ParticleEmitter& pe = AddEmitter(peSettings);

			if (emitter.contains("Material"))
			{
				pe.SetMaterial(AssetManager::Get().GetAsset<MaterialAsset>(emitter["Material"].get<std::string>())->material);
			}
		}
	}

	return true;
}
