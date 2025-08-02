#include "GraphicsEngine.pch.h"
#include "ParticleEmitter.h"
#include "Math/Random.hpp"
#include "Objects/DynamicVertexBuffer.h"

ParticleEmitter::ParticleEmitter()
{
}

ParticleEmitter::~ParticleEmitter()
{
}

void ParticleEmitter::Update(float aDeltaTime)
{
	for (size_t i = 0; i < myParticles.size(); i++)
	{
		ParticleVertex& particle = myParticles[i];
		particle.Lifetime += aDeltaTime;

		if (particle.Lifetime < 0) continue;

		if (particle.Lifetime >= mySettings.Lifetime)
		{
			InitParticle(particle, i);
		}
		
		UpdateParticle(particle, aDeltaTime);
	}

	myVertexBuffer->UpdateVertexBuffer(myParticles);
}

void ParticleEmitter::InitParticle(ParticleVertex& aParticle, size_t aIndex)
{
	const float spawnRateInv = 1.0f / mySettings.SpawnRate;
	aParticle.Lifetime = 0.0f - static_cast<float>(aIndex) * spawnRateInv;

	Math::Vector3f emitterHalfSize = mySettings.EmitterSize * 0.5f;
	aParticle.Position = Math::ToVector4(Random(-emitterHalfSize, emitterHalfSize), 1.0f);

	aParticle.Velocity = mySettings.Velocity;
	if (mySettings.RandomVelocity)
	{
		aParticle.Velocity = Random(mySettings.VelocityRangeMin, mySettings.VelocityRangeMax);
	}

	aParticle.Color = mySettings.Color.Get(0);
	aParticle.Size = mySettings.Size.Get(0);
	aParticle.GravityScale = mySettings.GravityScale;
	aParticle.Angle = mySettings.Angle.Get(0);
	aParticle.ChannelMask = mySettings.ChannelMask;
}

void ParticleEmitter::UpdateParticle(ParticleVertex& aParticle, float aDeltaTime)
{
	float lifeTimePercentage = aParticle.Lifetime / mySettings.Lifetime;

	aParticle.Velocity += Math::Vector3f(0, -9.81f * mySettings.GravityScale * aDeltaTime, 0);
	aParticle.Position += Math::ToVector4(aParticle.Velocity * aDeltaTime);
	aParticle.Angle = mySettings.Angle.Get(lifeTimePercentage);
	aParticle.Size = mySettings.Size.Get(lifeTimePercentage);
	aParticle.Color = mySettings.Color.Get(lifeTimePercentage);
}

void ParticleEmitter::InitInternal()
{
	const size_t numParticles = static_cast<size_t>(ceilf(mySettings.SpawnRate * mySettings.Lifetime));
	myParticles.resize(numParticles);

	for (size_t i = 0; i < numParticles; ++i)
	{
		ParticleVertex& p = myParticles[i];
		InitParticle(p, i);
	}

	myVertexBuffer = std::make_shared<DynamicVertexBuffer>();
	myVertexBuffer->CreateBuffer("Particle_VertexBuffer", myParticles, GraphicsSettings::PARTICLE_BUFFER_VERTEX_COUNT);
}