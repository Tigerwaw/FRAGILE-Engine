#pragma once
#include "Math/Vector.hpp"

#include "RHI/RHIStructs.h"

struct Vertex
{
	Math::Vector4f Position = { 0, 0, 0, 1 };
	Math::Vector4f VertexColor0 = { 0, 0, 0, 1 };
	Math::Vector4f VertexColor1 = { 0, 0, 0, 1 };
	Math::Vector4f VertexColor2 = { 0, 0, 0, 1 };
	Math::Vector4f VertexColor3 = { 0, 0, 0, 1 };
	Math::Vector2f TexCoord0;
	Math::Vector2f TexCoord1;
	Math::Vector2f TexCoord2;
	Math::Vector2f TexCoord3;
	Math::Vector3f Normal = { 0, 0, 0 };
	Math::Vector3f Tangent = { 0, 0, 0 };
	Math::Vector4<unsigned> BoneIDs;
	Math::Vector4f SkinWeights;

	Vertex(float position[4], float vertexColor[4][4], float UVs[4][2], float normal[3], float tangent[3])
	{
		Position		= { position[0], position[1], position[2], 1};
		VertexColor0	= { vertexColor[0][0], vertexColor[0][1], vertexColor[0][2], vertexColor[0][3] };
		VertexColor1	= { vertexColor[1][0], vertexColor[1][1], vertexColor[1][2], vertexColor[1][3] };
		VertexColor2	= { vertexColor[2][0], vertexColor[2][1], vertexColor[2][2], vertexColor[2][3] };
		VertexColor3	= { vertexColor[3][0], vertexColor[3][1], vertexColor[3][2], vertexColor[3][3] };
		TexCoord0		= { UVs[0][0], UVs[0][1] };
		TexCoord1		= { UVs[1][0], UVs[1][1] };
		TexCoord2		= { UVs[2][0], UVs[2][1] };
		TexCoord3		= { UVs[3][0], UVs[3][1] };
		Normal			= { normal[0], normal[1], normal[2] };
		Tangent			= { tangent[0], tangent[1], tangent[2] };
	}

	Vertex(float position[4], float vertexColor[4][4], unsigned boneIDs[4], float skinWeights[4], float UVs[4][2], float normal[3], float tangent[3])
	{
		Position		= { position[0], position[1], position[2], 1};
		VertexColor0	= { vertexColor[0][0], vertexColor[0][1], vertexColor[0][2], vertexColor[0][3] };
		VertexColor1	= { vertexColor[1][0], vertexColor[1][1], vertexColor[1][2], vertexColor[1][3] };
		VertexColor2	= { vertexColor[2][0], vertexColor[2][1], vertexColor[2][2], vertexColor[2][3] };
		VertexColor3	= { vertexColor[3][0], vertexColor[3][1], vertexColor[3][2], vertexColor[3][3] };
		TexCoord0		= { UVs[0][0], UVs[0][1] };
		TexCoord1		= { UVs[1][0], UVs[1][1] };
		TexCoord2		= { UVs[2][0], UVs[2][1] };
		TexCoord3		= { UVs[3][0], UVs[3][1] };
		Normal			= { normal[0], normal[1], normal[2] };
		Tangent			= { tangent[0], tangent[1], tangent[2] };
		BoneIDs			= { boneIDs[0], boneIDs[1], boneIDs[2], boneIDs[3] };
		SkinWeights		= { skinWeights[0], skinWeights[1], skinWeights[2], skinWeights[3] };
	}

	static const std::vector<VertexElementDesc> InputLayoutDefinition;
};

