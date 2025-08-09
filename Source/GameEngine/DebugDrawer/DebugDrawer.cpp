#include "Enginepch.h"

#include "DebugDrawer.h"
#include "GraphicsEngine.h"
#include "Objects/DynamicVertexBuffer.h"
#include "GraphicsCommands/GraphicsCommandList.h"
#include "EngineDefines.h"
#include "ComponentSystem/Components/Graphics/Camera.h"
#include "ComponentSystem/Components/Graphics/Model.h"
#include "ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "ComponentSystem/Components/Graphics/InstancedModel.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"
#include "Math/MathConstants.hpp"

void DebugDrawer::InitializeDebugDrawer()
{
    myLineVertices.emplace_back(DebugLineVertex({ 0, 0, 0 }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }));

    myLineBuffer = std::make_shared<DynamicVertexBuffer>();
    if (!myLineBuffer->CreateBuffer("Debug_Line_Buffer", myLineVertices, MAX_DEBUG_LINES))
    {
        LOG(LogDebugDrawer, Error, "Failed to create debug line buffer!");
    }

    myLineVertices.clear();
}

void DebugDrawer::DrawObjects()
{
    if (!myLineVertices.empty())
    {
        GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<BeginEvent>("Draw Debug Lines");
        GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::DebugLine));
        GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderDebugLines>(myLineVertices, myLineBuffer);
        GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<EndEvent>();
    }
}

void DebugDrawer::ClearObjects()
{
    if (!myLineVertices.empty())
    {
        myLineVertices.clear();
    }
}

void DebugDrawer::DrawLine(Math::Vector3f aFromPosition, Math::Vector3f aToPosition, Math::Vector4f aColor)
{
    if (myLineVertices.size() >= MAX_DEBUG_LINES)
    {
        if (!myHasWarned)
        {
            myHasWarned = true;
            LOG(LogDebugDrawer, Warning, "Debug Drawer Lines has exceeded its limit and all lines are not being drawn! Either draw less lines or consider increasing the max amount in engine defines.");
        }
        
        return;
    }

    myLineVertices.emplace_back(DebugLineVertex(aFromPosition, aToPosition, aColor));
}

void DebugDrawer::DrawLine(DebugLine aLine)
{
    DrawLine(aLine.From, aLine.To, aLine.Color);
}

void DebugDrawer::DrawCameraFrustum(std::shared_ptr<Camera> aCamera, Math::Vector4f aColor)
{
    Math::Matrix4x4f camMatrix = aCamera->gameObject->GetComponent<Transform>()->GetMatrix();
    std::array<Math::Vector3f, 8> frustumVolume = aCamera->GetFrustumCorners();

    for (auto& pos : frustumVolume)
    {
        pos = Math::ToVector3(Math::ToVector4(pos, 1.0f) * camMatrix);
    }

    // Near plane
    DrawLine(frustumVolume[0], frustumVolume[1], aColor);
    DrawLine(frustumVolume[1], frustumVolume[2], aColor);
    DrawLine(frustumVolume[2], frustumVolume[3], aColor);
    DrawLine(frustumVolume[3], frustumVolume[0], aColor);

    // Far Plane
    DrawLine(frustumVolume[4], frustumVolume[5], aColor);
    DrawLine(frustumVolume[5], frustumVolume[6], aColor);
    DrawLine(frustumVolume[6], frustumVolume[7], aColor);
    DrawLine(frustumVolume[7], frustumVolume[4], aColor);

    // Connecting Lines
    DrawLine(frustumVolume[0], frustumVolume[4], aColor);
    DrawLine(frustumVolume[1], frustumVolume[5], aColor);
    DrawLine(frustumVolume[2], frustumVolume[6], aColor);
    DrawLine(frustumVolume[3], frustumVolume[7], aColor);
}

void DebugDrawer::DrawBoundingBox(std::shared_ptr<Model> aModel, Math::Vector4f aColor)
{
    Math::Matrix4x4f objectMatrix = aModel->gameObject->GetComponent<Transform>()->GetWorldMatrix();
    Math::AABB3D<float> boundingBox = aModel->GetBoundingBox();
    DrawBoundingBox(boundingBox, objectMatrix, aColor);
}

void DebugDrawer::DrawBoundingBox(std::shared_ptr<AnimatedModel> aModel, Math::Vector4f aColor)
{
    Math::Matrix4x4f objectMatrix = aModel->gameObject->GetComponent<Transform>()->GetWorldMatrix();
    Math::AABB3D<float> boundingBox = aModel->GetBoundingBox();
    DrawBoundingBox(boundingBox, objectMatrix, aColor);
}

void DebugDrawer::DrawBoundingBox(std::shared_ptr<InstancedModel> aModel, Math::Vector4f aColor)
{
    Math::Matrix4x4f objectMatrix = aModel->gameObject->GetComponent<Transform>()->GetWorldMatrix();
    Math::AABB3D<float> boundingBox = aModel->GetBoundingBox();
    DrawBoundingBox(boundingBox, objectMatrix, aColor);
}

void DebugDrawer::DrawBoundingSphere(Math::Sphere<float> aSphere, Math::Matrix4x4f aWorldMatrix, Math::Vector4f aColor)
{
    int numPoints = 20;
    float angleinc = 2.0f * Math::PI / static_cast<float>(numPoints);
    Math::Sphere<float> sphereInNewSpace = aSphere.GetSphereinNewSpace(aWorldMatrix);
    Math::Vector3f center = sphereInNewSpace.GetPoint();
    float radius = sphereInNewSpace.GetRadius();

    {
        Math::Vector3f previousPointRight;
        Math::Vector3f nextPointRight = center;
        nextPointRight.x += radius;
        Math::Vector3f previousPointUp;
        Math::Vector3f nextPointUp = center;
        nextPointUp.y += radius;
        Math::Vector3f previousPointForward;
        Math::Vector3f nextPointForward = center;
        nextPointForward.z += radius;

        for (int i = 0; i <= numPoints; i++)
        {
            previousPointRight = nextPointRight;
            previousPointUp = nextPointUp;
            previousPointForward = nextPointForward;

            float c = cos(angleinc * i) * radius;
            float s = sin(angleinc * i) * radius;

            nextPointRight = center + Math::Vector3f(c, s, 0);
            nextPointUp = center + Math::Vector3f(0, c, s);
            nextPointForward = center + Math::Vector3f(s, 0, c);
            DrawLine(previousPointRight, nextPointRight, aColor);
            DrawLine(previousPointUp, nextPointUp, aColor);
            DrawLine(previousPointForward, nextPointForward, aColor);
        }
    }
}

void DebugDrawer::DrawBoundingBox(Math::AABB3D<float> aAABB, Math::Matrix4x4f aWorldMatrix, Math::Vector4f aColor)
{
    std::array<Math::Vector3f, 8> corners = aAABB.GetCorners();

    for (auto& corner : corners)
    {
        corner = Math::ToVector3(Math::ToVector4(corner, 1.0f) * aWorldMatrix);
    }

    DrawLine(corners[0], corners[1], aColor);
    DrawLine(corners[1], corners[2], aColor);
    DrawLine(corners[2], corners[3], aColor);
    DrawLine(corners[3], corners[0], aColor);

    DrawLine(corners[4], corners[5], aColor);
    DrawLine(corners[5], corners[6], aColor);
    DrawLine(corners[6], corners[7], aColor);
    DrawLine(corners[7], corners[4], aColor);

    DrawLine(corners[0], corners[4], aColor);
    DrawLine(corners[1], corners[5], aColor);
    DrawLine(corners[2], corners[6], aColor);
    DrawLine(corners[3], corners[7], aColor);
}
