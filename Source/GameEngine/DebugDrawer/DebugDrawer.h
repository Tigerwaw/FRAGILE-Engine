#pragma once
#include "Math/Matrix4x4.hpp"
#include "Math/Vector.hpp"
#include "Math/AABB3D.hpp"
#include "Math/Sphere.hpp"

#include "Objects/Vertices/DebugLineVertex.h"
#include "DebugDrawer/DebugLine.hpp"

class Camera;
class Model;
class AnimatedModel;
class InstancedModel;
class ParticleSystem;
class TrailSystem;

class DynamicVertexBuffer;

class DebugDrawer
{
public:
    void InitializeDebugDrawer();
    void DrawObjects();
    void ClearObjects();
    void DrawLine(Math::Vector3f aFromPosition, Math::Vector3f aToPosition, Math::Vector4f aColor = { 1.0f, 1.0f, 1.0f, 1.0f });
    void DrawLine(DebugLine aLine);
    void DrawCameraFrustum(std::shared_ptr<Camera> aCamera, Math::Vector4f aColor = { 0.0f, 0.0f, 1.0f, 1.0f });
    void DrawBoundingBox(Math::AABB3D<float> aAABB, Math::Matrix4x4f aWorldMatrix = Math::Matrix4x4f(), Math::Vector4f aColor = { 1.0f, 1.0f, 1.0f, 1.0f });
    void DrawBoundingBox(std::shared_ptr<Model> aModel, Math::Vector4f aColor = { 1.0f, 1.0f, 1.0f, 1.0f });
    void DrawBoundingBox(std::shared_ptr<AnimatedModel> aModel, Math::Vector4f aColor = { 1.0f, 1.0f, 1.0f, 1.0f });
    void DrawBoundingBox(std::shared_ptr<InstancedModel> aModel, Math::Vector4f aColor = { 1.0f, 1.0f, 1.0f, 1.0f });
    void DrawBoundingBox(std::shared_ptr<ParticleSystem> aParticleSystem, Math::Vector4f aColor = { 1.0f, 1.0f, 1.0f, 1.0f });
    void DrawBoundingBox(std::shared_ptr<TrailSystem> aTrailSystem, Math::Vector4f aColor = { 1.0f, 1.0f, 1.0f, 1.0f });
    void DrawBoundingSphere(Math::Sphere<float> aSphere, Math::Matrix4x4f aWorldMatrix = Math::Matrix4x4f(), Math::Vector4f aColor = { 1.0f, 1.0f, 1.0f, 1.0f });
private:

    std::vector<DebugLineVertex> myLineVertices;
    std::shared_ptr<DynamicVertexBuffer> myLineBuffer;

    bool myHasWarned = false;
};

