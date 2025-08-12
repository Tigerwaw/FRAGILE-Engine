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
    void DrawLine(const Math::Vector3f& aFromPosition, const Math::Vector3f& aToPosition, const Math::Vector4f& aColor);
    void DrawLine(const DebugLine& aLine);
    void DrawCameraFrustum(std::shared_ptr<Camera> aCamera, const Math::Vector4f& aColor);
    void DrawBoundingBox(const Math::AABB3D<float>& aAABB, const Math::Matrix4x4f& aWorldMatrix, const Math::Vector4f& aColor);
    void DrawBoundingSphere(const Math::Sphere<float>& aSphere, const Math::Matrix4x4f& aWorldMatrix, const Math::Vector4f& aColor);
    void DrawBoundingBox(std::shared_ptr<Model> aModel, const Math::Vector4f& aColor);
    void DrawBoundingBox(std::shared_ptr<AnimatedModel> aModel, const Math::Vector4f& aColor);
    void DrawBoundingBox(std::shared_ptr<InstancedModel> aModel, const Math::Vector4f& aColor);
    void DrawBoundingBox(std::shared_ptr<ParticleSystem> aParticleSystem, const Math::Vector4f& aColor);
    void DrawBoundingBox(std::shared_ptr<TrailSystem> aTrailSystem, const Math::Vector4f& aColor);
private:

    std::vector<DebugLineVertex> myLineVertices;
    std::shared_ptr<DynamicVertexBuffer> myLineBuffer;

    bool myHasWarned = false;
};

