#include "GraphicsEngine.pch.h"
#include "TrailEmitter.h"
#include "Objects/DynamicVertexBuffer.h"

TrailEmitter::TrailEmitter()
{
}

TrailEmitter::~TrailEmitter()
{
}

void TrailEmitter::Update(Math::Vector3f aFollowTarget, float aDeltaTime)
{
	myBoundingBox.InitWithMinAndMax(aFollowTarget, aFollowTarget);

	myCurrentLength = 0;
	aFollowTarget += mySettings.ConstantVelocity * aDeltaTime;
	myTrailVertices[0].Position = Math::ToVector4(aFollowTarget, 1.0f);
	myPreviousPositions[0] = aFollowTarget;
	UpdateTrailVertex(myTrailVertices[0], aDeltaTime, 0);
	UpdateBoundingBox(Math::ToVector3(myTrailVertices[0].Position), myTrailVertices[0].Width);

	for (unsigned i = 1; i < static_cast<unsigned>(myTrailVertices.size()); i++)
	{
		TrailVertex& trailVertex = myTrailVertices[i];
		trailVertex.Position = Math::ToVector4(myPreviousPositions[i * mySettings.Length], 1.0f);
		UpdateBoundingBox(Math::ToVector3(trailVertex.Position), trailVertex.Width);
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
	UpdateBoundingBox(Math::ToVector3(aTrailVertex.Position), aTrailVertex.Width);
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
	myVertexBuffer->CreateBuffer("Trail_VertexBuffer", myTrailVertices, GraphicsSettings::TRAIL_BUFFER_VERTEX_COUNT);
}

void TrailEmitter::UpdateBoundingBox(const Math::Vector3f& aPosition, float aWidth)
{
	Math::Vector3f bbMin = myBoundingBox.GetMin();
	Math::Vector3f bbMax = myBoundingBox.GetMax();

	bbMin.x = std::fminf(aPosition.x - aWidth, bbMin.x);
	bbMax.x = std::fmaxf(aPosition.x + aWidth, bbMax.x);
	bbMin.y = std::fminf(aPosition.y - aWidth, bbMin.y);
	bbMax.y = std::fmaxf(aPosition.y + aWidth, bbMax.y);
	bbMin.z = std::fminf(aPosition.z - aWidth, bbMin.z);
	bbMax.z = std::fmaxf(aPosition.z + aWidth, bbMax.z);

	myBoundingBox.InitWithMinAndMax(bbMin, bbMax);
}
