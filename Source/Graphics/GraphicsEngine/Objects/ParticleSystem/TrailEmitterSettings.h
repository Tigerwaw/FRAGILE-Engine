#pragma once
#include "Math/Vector.hpp"
#include "Math/Curve.h"


struct TrailEmitterSettings
{
	unsigned Vertices = 2;
	Math::Curve<Math::Vector4f> Color;
	Math::Curve<float> Width;
	unsigned Length = 10;
	Math::Vector4f ChannelMask = { 1.0f, 1.0f, 1.0f, 1.0f };
	Math::Vector3f ConstantVelocity;
};
