#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Matrix4x4.hpp"
#include "Objects/ParticleSystem/ParticleEmitter.h"

class RenderParticles : public GraphicsCommandBase
{
public:
	RenderParticles(const std::vector<ParticleEmitter>& aEmitters, const Math::Matrix4x4f& aTransform);
	void Execute() override;
	void Destroy() override;
private:
	std::vector<ParticleEmitter> myEmitters;
	Math::Matrix4x4f myTransform;
};

