#pragma once
#include "GameEngine/ComponentSystem/Component.h"
#include "Math/Vector3.hpp"

class BounceAgainstWorldEdges : public Component
{
public:
	void Start() override {}
	void Update() override;

private:
	Math::Vector3f myExtents = { 800.0f, 0.0f, 800.0f };
};