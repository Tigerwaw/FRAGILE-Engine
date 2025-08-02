#pragma once
#include "Math/Vector.hpp"


#include "RHI/RHIStructs.h"

struct TextVertex
{
	Math::Vector4f Position;
	Math::Vector2f TexCoords;

	TextVertex(float positionX, float positionY, float uvX, float uvY)
	{
		Position = { positionX, positionY, 0, 1.0f };
		TexCoords = { uvX, uvY };
	}

	static const std::vector<VertexElementDesc> InputLayoutDefinition;
};

