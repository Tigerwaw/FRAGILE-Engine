#pragma once
#include "Math/AABB3D.hpp"
#include "Math/Sphere.hpp"

class GameObject;
class Scene;
class Camera;
class Transform;
class AmbientLight;
class DirectionalLight;
class PointLight;
class SpotLight;

// TEMP
class Sprite;
class Text;

class RenderAssembler
{
public:
    RenderAssembler();
    ~RenderAssembler();
    void RenderScene(Scene& aScene);

    // TEMP
    void Init();
private:
    struct SceneRenderData
    {
        Math::AABB3D<float> sceneBoundingBox;
        Camera* mainCamera;
        std::shared_ptr<AmbientLight> ambientLight;
        std::shared_ptr<DirectionalLight> directionalLight;
        std::vector<std::shared_ptr<PointLight>> pointLights;
        std::vector<std::shared_ptr<SpotLight>> spotLights;

        std::vector<std::shared_ptr<GameObject>> castShadowsStatic;
        std::vector<std::shared_ptr<GameObject>> castShadowsDynamic;
        std::vector<std::shared_ptr<GameObject>> drawDeferred;
        std::vector<std::shared_ptr<GameObject>> drawForward;
        std::vector<std::shared_ptr<GameObject>> drawParticleSystems;
        std::vector<std::shared_ptr<GameObject>> drawText;

        std::vector<std::shared_ptr<GameObject>> drawCollidersObjects;
        std::vector<std::shared_ptr<GameObject>> drawBoundingBoxesObjects;
        std::vector<std::shared_ptr<GameObject>> drawCameraFrustumsObjects;
    };

    SceneRenderData AssembleLists(Scene& aScene);
    void SortRenderables(SceneRenderData& aRenderData);

    void RenderDebug(SceneRenderData& aRenderData);
    void RenderDeferred(SceneRenderData& aRenderData);

    void QueueDeferredLightPasses(SceneRenderData& aRenderData);
    void QueueDeferredObjects(SceneRenderData& aRenderData);
    void QueueForwardObjects(SceneRenderData& aRenderData);
    void QueueShadowmapTextureResources(bool aUseStaticShadowmaps, SceneRenderData& aRenderData);
    void QueueUpdateLightBuffer(SceneRenderData& aRenderData);
    void QueueSpotLightShadows(SceneRenderData& aRenderData);
    void QueuePointLightShadows(SceneRenderData& aRenderData);
    void QueueDirectionalLightShadows(SceneRenderData& aRenderData);
    void QueueObjectShadows(const std::vector<std::shared_ptr<GameObject>>& aGameObjects, std::shared_ptr<Camera> aRenderCamera);
    void QueueObjectShadows(const std::vector<std::shared_ptr<GameObject>>& aGameObjects, std::shared_ptr<PointLight> aPointLight);
    void QueueObjectsDebug(SceneRenderData& aRenderData);
    void QueueDebugLines(SceneRenderData& aRenderData);

    bool IsInsideFrustum(Camera* aRenderCamera, const Math::AABB3D<float>& aObjectAABB);
    bool IsInsideFrustum(Camera* aRenderCamera, std::shared_ptr<Transform> aObjectTransform, const Math::AABB3D<float>& aObjectAABB);
    bool IsInsideFrustum(Camera* aRenderCamera, std::shared_ptr<Transform> aObjectTransform, const std::array<Math::Vector3f, 8>& aLightFrustum);
    bool IsInsideRadius(std::shared_ptr<PointLight> aPointLight, std::shared_ptr<Transform> aObjectTransform, const Math::AABB3D<float>& aObjectAABB);

    void UpdateBoundingBox(std::shared_ptr<Transform> aTransform, const Math::AABB3D<float>& aBoundingBox);
    Math::AABB3D<float> myVisibleObjectsBB;

public:
    bool myShouldUpdateStaticShadows = true;
private:

    // TEMP
    void DrawTestUI();
    std::shared_ptr<Sprite> myTestSprite;
};

