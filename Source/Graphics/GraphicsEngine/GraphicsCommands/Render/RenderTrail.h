#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Matrix4x4.hpp"
#include "Objects/ParticleSystem/TrailEmitter.h"

class RenderTrail : public GraphicsCommandBase
{
public:
	RenderTrail(const std::vector<TrailEmitter>& aEmitters, const Math::Matrix4x4f& aTransform);
	void Execute() override;
private:
	std::vector<TrailEmitter> myEmitters;
	Math::Matrix4x4f myTransform;
};

