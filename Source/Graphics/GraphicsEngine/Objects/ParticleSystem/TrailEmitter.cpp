#include "GraphicsEngine.pch.h"
#include "TrailEmitter.h"
#include "Objects/DynamicVertexBuffer.h"

#define MAX_PARTICLES 20000

TrailEmitter::TrailEmitter()
{
}

TrailEmitter::~TrailEmitter()
{
}

void TrailEmitter::Update(Math::Vector3f aFollowTarget, float aDeltaTime)
{
	myCurrentLength = 0;
	aFollowTarget += mySettings.ConstantVelocity * aDeltaTime;
	myTrailVertices[0].Position = Math::ToVector4(aFollowTarget, 1.0f);
	myPreviousPositions[0] = aFollowTarget;
	UpdateTrailVertex(myTrailVertices[0], aDeltaTime, 0);

	for (unsigned i = 1; i < static_cast<unsigned>(myTrailVertices.size()); i++)
	{
		TrailVertex& trailVertex = myTrailVertices[i];
		trailVertex.Position = Math::ToVector4(myPreviousPositions[i * mySettings.Length], 1.0f);
		if (trailVertex.Position.LengthSqr() > 1.0f)
		{
			myCurrentLength++;
		}

		UpdateTrailVertex(trailVertex, aDeltaTime, i);
	}

	for (size_t i = myPreviousPositions.size() - 1; i > 0; --i)
	{
		myPreviousPositions[i] = myPreviousPositions[i - 1];
		myPreviousPositions[i] += mySettings.ConstantVelocity * aDeltaTime;
	}

	myVertexBuffer->UpdateVertexBuffer(myTrailVertices);
}

void TrailEmitter::ResetTrail()
{
	for (auto& pos : myPreviousPositions)
	{
		pos = Math::Vector3f();
	}

	for (auto& vertex : myTrailVertices)
	{
		InitTrailVertex(vertex);
	}
}

void TrailEmitter::InitTrailVertex(TrailVertex& aTrailVertex)
{
	aTrailVertex.Color = mySettings.Color.Get(0);
	aTrailVertex.Width = mySettings.Width.Get(0);
	aTrailVertex.ChannelMask = mySettings.ChannelMask;
	aTrailVertex.Lifetime = 0.0f;
	aTrailVertex.Position = Math::Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
}

void TrailEmitter::UpdateTrailVertex(TrailVertex& aTrailVertex, float aDeltaTime, unsigned aIndex)
{
	aDeltaTime;

	float t = static_cast<float>(aIndex) / static_cast<float>(myTrailVertices.size());
	aTrailVertex.Width = mySettings.Width.Get(t);
	aTrailVertex.Color = mySettings.Color.Get(t);
}

void TrailEmitter::InitInternal()
{
	const size_t numTrailVertices = mySettings.Vertices;
	myTrailVertices.resize(numTrailVertices);

	myPreviousPositions.resize(numTrailVertices * mySettings.Length);

	for (size_t i = 0; i < numTrailVertices; ++i)
	{
		TrailVertex& p = myTrailVertices[i];
		InitTrailVertex(p);
	}

	myVertexBuffer = std::make_shared<DynamicVertexBuffer>();
	myVertexBuffer->CreateBuffer("Trail_VertexBuffer", myTrailVertices, MAX_PARTICLES);
}