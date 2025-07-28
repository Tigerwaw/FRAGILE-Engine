#pragma once
#include "Math/Vector.hpp"

struct MaterialBuffer
{
	Math::Vector4<float> albedoTint = { 1.0f, 1.0f, 1.0f, 1.0f };
	float emissiveStrength = 0.0f;
	Math::Vector3f padding;
	Math::Vector2f uvScale = { 1.0f, 1.0f };
	Math::Vector2f uvOffset = { 0.0f, 0.0f };
};