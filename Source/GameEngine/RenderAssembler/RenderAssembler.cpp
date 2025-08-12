#include "Enginepch.h"
#include "RenderAssembler.h"

#include "Engine.h"
#include "DebugDrawer/DebugDrawer.h"
#include "AssetManager.h"
#include "GraphicsEngine.h"
#include "Objects/Sprite.h"
#include "Objects/Text/Text.h"
#include "Objects/Material.h"
#include "Math/Vector.hpp"
#include "Math/AABB3D.hpp"
#include "Math/Intersection3D.hpp"
#include "Time/Timer.h"

#include "ComponentSystem/Scene.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"
#include "ComponentSystem/Components/Graphics/Model.h"
#include "ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "ComponentSystem/Components/Graphics/InstancedModel.h"
#include "ComponentSystem/Components/Graphics/ParticleSystem.h"
#include "ComponentSystem/Components/Graphics/TrailSystem.h"
#include "ComponentSystem/Components/Graphics/Camera.h"
#include "ComponentSystem/Components/Lights/AmbientLight.h"
#include "ComponentSystem/Components/Lights/DirectionalLight.h"
#include "ComponentSystem/Components/Lights/PointLight.h"
#include "ComponentSystem/Components/Lights/SpotLight.h"
#include "ComponentSystem/Components/Physics/Colliders/BoxCollider.h"
#include "ComponentSystem/Components/Physics/Colliders/SphereCollider.h"
#include "ComponentSystem/Components/Graphics/TextComponent.h"

#include "AssetTypes/TextureAsset.h"
#include "AssetTypes/PSOAsset.h"
#include "AssetTypes/FontAsset.h"


RenderAssembler::RenderAssembler() = default;
RenderAssembler::~RenderAssembler() = default;

void RenderAssembler::RenderScene(Scene& aScene)
{
	PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Add Render Commands");

	SceneRenderData sceneRenderData = AssembleLists(aScene);
	SortRenderables(sceneRenderData);

	if (GraphicsEngine::Get().CurrentDebugRenderMode == DebugRenderMode::None)
	{
		RenderDeferred(sceneRenderData);
	}
	else
	{
		RenderDebug(sceneRenderData);
	}
}

RenderAssembler::SceneRenderData RenderAssembler::AssembleLists(Scene& aScene)
{
	PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Assemble Scene Data");

	SceneRenderData sceneRenderData;
	sceneRenderData.mainCamera = Camera::GetMainCamera();
	sceneRenderData.castShadowsStatic.reserve(100);
	sceneRenderData.castShadowsDynamic.reserve(100);
	sceneRenderData.drawDeferred.reserve(100);
	sceneRenderData.drawForward.reserve(100);
	sceneRenderData.pointLights.reserve(4);
	sceneRenderData.spotLights.reserve(4);
	if (Engine::Get().DrawColliders)
		sceneRenderData.drawCollidersObjects.reserve(100);
	if (Engine::Get().DrawBoundingBoxes)
		sceneRenderData.drawBoundingBoxesObjects.reserve(100);

	sceneRenderData.sceneBoundingBox = aScene.myBoundingBox;

	for (auto& gameObject : aScene.myGameObjects)
	{
		if (!gameObject->GetActive()) continue;

		{
			std::shared_ptr<Model> model = gameObject->GetComponent<Model>();
			if (model && model->GetActive())
			{
				if (model->GetCastShadows())
				{
					if (gameObject->GetStatic())
					{
						sceneRenderData.castShadowsStatic.emplace_back(gameObject);
					}
					else
					{
						sceneRenderData.castShadowsDynamic.emplace_back(gameObject);
					}
				}

				if (model->GetMaterials().size() > 0)
				{
					if (model->GetMaterialOnSlot(0)->GetPSO()->BlendState != nullptr)
					{
						sceneRenderData.drawForward.emplace_back(gameObject);
					}
					else
					{
						sceneRenderData.drawDeferred.emplace_back(gameObject);
					}
				}

				if (Engine::Get().DrawBoundingBoxes)
				{
					sceneRenderData.drawBoundingBoxesObjects.emplace_back(gameObject);
				}
			}
		}

		{
			std::shared_ptr<AnimatedModel> animModel = gameObject->GetComponent<AnimatedModel>();
			if (animModel && animModel->GetActive())
			{
				if (animModel->GetCastShadows())
				{
					if (gameObject->GetStatic())
					{
						sceneRenderData.castShadowsStatic.emplace_back(gameObject);
					}
					else
					{
						sceneRenderData.castShadowsDynamic.emplace_back(gameObject);
					}
				}

				if (animModel->GetMaterials().size() > 0)
				{
					if (animModel->GetMaterialOnSlot(0)->GetPSO()->BlendState != nullptr)
					{
						sceneRenderData.drawForward.emplace_back(gameObject);
					}
					else
					{
						sceneRenderData.drawDeferred.emplace_back(gameObject);
					}
				}

				if (Engine::Get().DrawBoundingBoxes)
				{
					sceneRenderData.drawBoundingBoxesObjects.emplace_back(gameObject);
				}
			}
		}

		{
			std::shared_ptr<InstancedModel> instancedModel = gameObject->GetComponent<InstancedModel>();
			if (instancedModel && instancedModel->GetActive())
			{
				if (instancedModel->GetCastShadows())
				{
					if (gameObject->GetStatic())
					{
						sceneRenderData.castShadowsStatic.emplace_back(gameObject);
					}
					else
					{
						sceneRenderData.castShadowsDynamic.emplace_back(gameObject);
					}
				}

				if (instancedModel->GetMaterials().size() > 0)
				{
					if (instancedModel->GetMaterialOnSlot(0)->GetPSO()->BlendState != nullptr)
					{
						sceneRenderData.drawForward.emplace_back(gameObject);
					}
					else
					{
						sceneRenderData.drawDeferred.emplace_back(gameObject);
					}
				}

				if (Engine::Get().DrawBoundingBoxes)
				{
					sceneRenderData.drawBoundingBoxesObjects.emplace_back(gameObject);
				}
			}
		}

		{
			std::shared_ptr<ParticleSystem> particleSystem = gameObject->GetComponent<ParticleSystem>();
			if (particleSystem && particleSystem->GetActive())
			{
				sceneRenderData.drawParticleSystems.emplace_back(gameObject);
			}

			std::shared_ptr<TrailSystem> trailSystem = gameObject->GetComponent<TrailSystem>();
			if (trailSystem && trailSystem->GetActive())
			{
				sceneRenderData.drawParticleSystems.emplace_back(gameObject);
			}
		}

		{
			std::shared_ptr<TextComponent> text = gameObject->GetComponent<TextComponent>();
			if (text && text->GetActive())
			{
				sceneRenderData.drawText.emplace_back(gameObject);
			}
		}

		{
			std::shared_ptr<AmbientLight> ambientLight = gameObject->GetComponent<AmbientLight>();
			if (ambientLight && ambientLight->GetActive())
			{
				sceneRenderData.ambientLight = ambientLight;
			}
		}

		{
			std::shared_ptr<DirectionalLight> directionalLight = gameObject->GetComponent<DirectionalLight>();
			if (directionalLight && directionalLight->GetActive())
			{
				sceneRenderData.directionalLight = directionalLight;
			}
		}

		{
			std::shared_ptr<PointLight> pointLight = gameObject->GetComponent<PointLight>();
			if (pointLight && pointLight->GetActive())
			{
				std::shared_ptr<Transform> pointLightTransform = gameObject->GetComponent<Transform>();
				std::shared_ptr<Camera> pointLightCam = gameObject->GetComponent<Camera>();

				Math::AABB3D<float> aabb(pointLightTransform->GetTranslation(), pointLightCam->GetFarPlane(), pointLightCam->GetFarPlane(), pointLightCam->GetFarPlane());

				if (IsInsideFrustum(sceneRenderData.mainCamera, pointLightTransform, aabb))
					sceneRenderData.pointLights.emplace_back(pointLight);
			}
		}

		{
			std::shared_ptr<SpotLight> spotLight = gameObject->GetComponent<SpotLight>();
			if (spotLight && spotLight->GetActive())
			{
				std::shared_ptr<Transform> spotLightTransform = gameObject->GetComponent<Transform>();
				std::shared_ptr<Camera> spotLightCam = gameObject->GetComponent<Camera>();

				if (IsInsideFrustum(sceneRenderData.mainCamera, spotLightTransform, spotLightCam->GetFrustumCorners()))
					sceneRenderData.spotLights.emplace_back(spotLight);
			}
		}

		std::shared_ptr<Camera> cam = gameObject->GetComponent<Camera>();
		if (cam && cam->GetActive())
		{
			if (!cam->IsMainCamera() && Engine::Get().DrawCameraFrustums)
			{
				sceneRenderData.drawCameraFrustumsObjects.emplace_back(gameObject);
			}
		}

		if (Engine::Get().DrawColliders)
		{
			std::shared_ptr<BoxCollider> boxCollider = gameObject->GetComponent<BoxCollider>();
			if (boxCollider && boxCollider->GetActive())
			{
				sceneRenderData.drawCollidersObjects.emplace_back(gameObject);
			}

			std::shared_ptr<SphereCollider> sphereCollider = gameObject->GetComponent<SphereCollider>();
			if (sphereCollider && sphereCollider->GetActive())
			{
				sceneRenderData.drawCollidersObjects.emplace_back(gameObject);
			}
		}
	}

	return sceneRenderData;
}

void RenderAssembler::SortRenderables(SceneRenderData& aRenderData)
{
	PIXScopedEvent(PIX_COLOR_INDEX(8), "Sort GameObjects in Scene");

	Math::Vector3f camPos = aRenderData.mainCamera->gameObject->GetComponent<Transform>()->GetTranslation(true);

	std::stable_sort(aRenderData.castShadowsStatic.begin(), aRenderData.castShadowsStatic.end(), [camPos](const std::shared_ptr<GameObject> lhs, const std::shared_ptr<GameObject> rhs)
		{
			std::shared_ptr<Transform> transform1 = lhs->GetComponent<Transform>();
			std::shared_ptr<Transform> transform2 = rhs->GetComponent<Transform>();
			if (transform1 && transform2)
			{
				float distTo1 = Math::Vector3f(camPos - transform1->GetTranslation(true)).LengthSqr();
				float distTo2 = Math::Vector3f(camPos - transform2->GetTranslation(true)).LengthSqr();

				return distTo1 < distTo2;
			}

			return false;
		});

	std::stable_sort(aRenderData.castShadowsDynamic.begin(), aRenderData.castShadowsDynamic.end(), [camPos](const std::shared_ptr<GameObject> lhs, const std::shared_ptr<GameObject> rhs)
		{
			std::shared_ptr<Transform> transform1 = lhs->GetComponent<Transform>();
			std::shared_ptr<Transform> transform2 = rhs->GetComponent<Transform>();
			if (transform1 && transform2)
			{
				float distTo1 = Math::Vector3f(camPos - transform1->GetTranslation(true)).LengthSqr();
				float distTo2 = Math::Vector3f(camPos - transform2->GetTranslation(true)).LengthSqr();

				return distTo1 < distTo2;
			}

			return false;
		});

	std::stable_sort(aRenderData.drawDeferred.begin(), aRenderData.drawDeferred.end(), [camPos](const std::shared_ptr<GameObject> lhs, const std::shared_ptr<GameObject> rhs)
		{
			std::shared_ptr<Transform> transform1 = lhs->GetComponent<Transform>();
			std::shared_ptr<Transform> transform2 = rhs->GetComponent<Transform>();
			if (transform1 && transform2)
			{
				float distTo1 = Math::Vector3f(camPos - transform1->GetTranslation(true)).LengthSqr();
				float distTo2 = Math::Vector3f(camPos - transform2->GetTranslation(true)).LengthSqr();

				return distTo1 < distTo2;
			}

			return false;
		});

	std::stable_sort(aRenderData.drawForward.begin(), aRenderData.drawForward.end(), [camPos](const std::shared_ptr<GameObject> lhs, const std::shared_ptr<GameObject> rhs)
		{
			std::shared_ptr<Transform> transform1 = lhs->GetComponent<Transform>();
			std::shared_ptr<Transform> transform2 = rhs->GetComponent<Transform>();
			if (transform1 && transform2)
			{
				float distTo1 = Math::Vector3f(camPos - transform1->GetTranslation(true)).LengthSqr();
				float distTo2 = Math::Vector3f(camPos - transform2->GetTranslation(true)).LengthSqr();

				return distTo1 > distTo2;
			}

			return false;
		});
}

void RenderAssembler::RenderDebug(SceneRenderData& aRenderData)
{
	GraphicsEngine& gfx = GraphicsEngine::Get();
	GraphicsCommandList& gfxList = gfx.GetGraphicsCommandList();

	gfxList.Enqueue<BeginEvent>("Rendering Debug Mode");

	// Final Render
	auto camTransform = aRenderData.mainCamera->gameObject->GetComponent<Transform>();
	FrameBuffer frameBuffer;
	frameBuffer.InvView = camTransform->GetWorldMatrix().GetFastInverse();
	frameBuffer.Projection = aRenderData.mainCamera->GetProjectionMatrix();
	frameBuffer.ViewPosition = Math::ToVector4(camTransform->GetTranslation(true), 1.0f);
	frameBuffer.ViewDirection = Math::ToVector4(camTransform->GetForwardVector(true), 1.0f);
	frameBuffer.NearPlane = aRenderData.mainCamera->GetNearPlane();
	frameBuffer.FarPlane = aRenderData.mainCamera->GetFarPlane();
	frameBuffer.Time = { static_cast<float>(Engine::Get().GetTimer().GetTimeSinceProgramStart()), Engine::Get().GetTimer().GetDeltaTime() };
	frameBuffer.Resolution = Engine::Get().GetResolution();
	frameBuffer.DebugRenderMode = static_cast<int>(GraphicsEngine::Get().CurrentDebugRenderMode);

	gfxList.Enqueue<UpdateFrameBuffer>(std::move(frameBuffer));

	std::shared_ptr<PipelineStateObject> pso;

	int debugMode = static_cast<int>(GraphicsEngine::Get().CurrentDebugRenderMode);
	if (debugMode <= 7)
	{
		gfxList.Enqueue<SetGBufferAsRenderTarget>();
		gfxList.Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::Deferred));
		QueueObjectsDebug(aRenderData);
		gfxList.Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::DebugGBuffer));
		gfxList.Enqueue<SetRenderTarget>(gfx.GetBackBuffer(), nullptr, true, false);
		gfxList.Enqueue<SetGBufferAsResource>();
		gfxList.Enqueue<RenderFullscreenQuad>();
		for (int i = 0; i < 5; i++)
		{
			gfxList.Enqueue<ClearTextureResource>(i);
		}
	}
	else if (debugMode == 8)
	{
		gfxList.Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::Wireframe));
		gfxList.Enqueue<SetDefaultRenderTarget>();
		QueueObjectsDebug(aRenderData);
	}
	else
	{
		gfxList.Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::DebugForward));
		gfxList.Enqueue<SetDefaultRenderTarget>();
		QueueObjectsDebug(aRenderData);
	}
}

void RenderAssembler::RenderDeferred(SceneRenderData& aRenderData)
{
	PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Deferred Pass");

	GraphicsEngine& gfx = GraphicsEngine::Get();
	PostProcessingSettings& ppSettings = GraphicsEngine::Get().GetPostProcessingSettings();
	GraphicsCommandList& gfxList = gfx.GetGraphicsCommandList();

	if (Engine::Get().RecalculateShadowFrustum)
	{
		aRenderData.directionalLight->RecalculateShadowFrustum(aRenderData.mainCamera, myVisibleObjectsBB);
		myVisibleObjectsBB.InitWithCenterAndExtents(Math::Vector3f(), Math::Vector3f());
	}
	
	{
		PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Queue Shadows");
		gfxList.Enqueue<BeginEvent>("Shadow Pass");
		gfxList.Enqueue<UpdatePostProcessBuffer>();
		QueueDirectionalLightShadows(aRenderData);
		QueuePointLightShadows(aRenderData);
		QueueSpotLightShadows(aRenderData);
		gfxList.Enqueue<EndEvent>();
	}

	// Deferred Objects
	{
		PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Draw Deferred Objects");
		gfxList.Enqueue<BeginEvent>("Draw Deferred Objects");
		gfxList.Enqueue<SetGBufferAsRenderTarget>();

		auto camTransform = aRenderData.mainCamera->gameObject->GetComponent<Transform>();
		FrameBuffer frameBuffer;
		frameBuffer.InvView = camTransform->GetWorldMatrix().GetFastInverse();
		frameBuffer.Projection = aRenderData.mainCamera->GetProjectionMatrix();
		frameBuffer.ViewPosition = Math::ToVector4(camTransform->GetTranslation(true), 1.0f);
		frameBuffer.ViewDirection = Math::ToVector4(camTransform->GetForwardVector(true), 1.0f);
		frameBuffer.NearPlane = aRenderData.mainCamera->GetNearPlane();
		frameBuffer.FarPlane = aRenderData.mainCamera->GetFarPlane();
		frameBuffer.Time = { static_cast<float>(Engine::Get().GetTimer().GetTimeSinceProgramStart()), Engine::Get().GetTimer().GetDeltaTime() };
		frameBuffer.Resolution = Engine::Get().GetResolution();
		gfxList.Enqueue<UpdateFrameBuffer>(std::move(frameBuffer));
		QueueDeferredObjects(aRenderData);
		gfxList.Enqueue<EndEvent>();
	}

	if (ppSettings.SSAOEnabled)
	{
		PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler SSAO");

		// Downsampled SSAO
		gfxList.Enqueue<BeginEvent>("SSAO Pass");
		gfxList.Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::SSAO));
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::HalfScreenA), nullptr, true, false);
		gfxList.Enqueue<SetGBufferAsResource>();
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<EndEvent>();

		// Blur
		gfxList.Enqueue<BeginEvent>("Blur Pass");
		gfxList.Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::RadialBlur));
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::Luminance), nullptr, true, false);
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::HalfScreenA));
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);
		gfxList.Enqueue<EndEvent>();
	}

	// Light passes
	{
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::HDR), nullptr, true, false); // Make a clear render target command lol

		QueueUpdateLightBuffer(aRenderData);
		QueueShadowmapTextureResources(true, aRenderData);
		QueueDeferredLightPasses(aRenderData);
		for (int i = 100; i < 110; i++)
		{
			gfxList.Enqueue<ClearTextureResource>(i);
		}

		QueueShadowmapTextureResources(false, aRenderData);
		QueueDeferredLightPasses(aRenderData);
		for (int i = 100; i < 110; i++)
		{
			gfxList.Enqueue<ClearTextureResource>(i);
		}

		// Clear GBuffer as Resource
		for (int i = 0; i < 5; i++)
		{
			gfxList.Enqueue<ClearTextureResource>(i);
		}

		myShouldUpdateStaticShadows = false;
	}

	{
		PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Draw Skybox");
		gfxList.Enqueue<BeginEvent>("Draw Skybox");
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::HDR), gfx.GetDepthBuffer(), false, false);

		auto transform = aRenderData.ambientLight->gameObject->GetComponent<Transform>()->GetWorldMatrix();
		auto mainCamTransform = aRenderData.mainCamera->gameObject->GetComponent<Transform>()->GetWorldMatrix();
		transform(4, 1) = mainCamTransform(4, 1);
		transform(4, 2) = mainCamTransform(4, 2);
		transform(4, 3) = mainCamTransform(4, 3);
		gfxList.Enqueue<RenderSkybox>(aRenderData.ambientLight->GetSkyboxMesh(), aRenderData.ambientLight->GetCubemap(), transform);

		gfxList.Enqueue<EndEvent>();
	}
	
	// Forward objects
	{
		PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Draw Forward Objects");
		gfxList.Enqueue<BeginEvent>("Draw Forward Objects");
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::HDR), gfx.GetDepthBuffer(), false, false);
		QueueForwardObjects(aRenderData);
		gfxList.Enqueue<EndEvent>();
	}

	// Draw Text
	{
		PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Draw Text");
		gfxList.Enqueue<BeginEvent>("Draw Text");
		gfx.GetGraphicsCommandList().Enqueue<ChangePipelineState>(gfx.GetPSO(PSOType::Text));
		for (auto& gameObject : aRenderData.drawText)
		{
			std::shared_ptr<TextComponent> text = gameObject->GetComponent<TextComponent>();
			if (text)
			{
				gfx.GetGraphicsCommandList().Enqueue<RenderText>(text->GetText(), text->gameObject->GetComponent<Transform>()->GetWorldMatrix());
			}
		}

		gfxList.Enqueue<EndEvent>();
	}

	{
		PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Draw Particle & Trail Systems");
		gfxList.Enqueue<BeginEvent>("Draw Particle & Trail Systems");

		for (auto& gameObject : aRenderData.drawParticleSystems)
		{
			std::shared_ptr<ParticleSystem> particleSystem = gameObject->GetComponent<ParticleSystem>();
			if (particleSystem && particleSystem->GetActive())
			{
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderParticles>(particleSystem->GetEmitters(), gameObject->GetComponent<Transform>()->GetWorldMatrix());
			}

			std::shared_ptr<TrailSystem> trailSystem = gameObject->GetComponent<TrailSystem>();
			if (trailSystem && trailSystem->GetActive())
			{
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderTrail>(trailSystem->GetEmitters(), gameObject->GetComponent<Transform>()->GetWorldMatrix());
			}
		}

		gfxList.Enqueue<EndEvent>();
	}

	QueueDebugLines(aRenderData);
	Engine::Get().GetDebugDrawer().DrawObjects();

	std::shared_ptr<Texture> renderTarget;
	if (ppSettings.BloomEnabled)
	{
		renderTarget = gfx.GetIntermediateTexture(IntermediateTexture::LDR);
	}
	else
	{
		renderTarget = gfx.GetBackBuffer();
	}

	// Tonemapping
	{
		PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Tonemapping");
		gfxList.Enqueue<BeginEvent>("Tonemapping Pass");
		gfxList.Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::TonemapACES)); // Fix functionality for switching tonemapper
		gfxList.Enqueue<SetRenderTarget>(renderTarget, nullptr, true, false);
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::HDR));
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);
		gfxList.Enqueue<EndEvent>();
	}

	if (ppSettings.BloomEnabled)
	{
		PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Bloom Pass");

		// Luminance
		gfxList.Enqueue<BeginEvent>("Luminance Pass");
		gfxList.Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::Luminance));
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::Luminance), nullptr, true, false);
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::LDR));
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);
		gfxList.Enqueue<EndEvent>();

		// Downsample
		gfxList.Enqueue<BeginEvent>("Downsampling Pass");
		gfxList.Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::Resample));
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::HalfScreenA), nullptr, true, false);
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::Luminance));
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);

		gfxList.Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::Resample));
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::QuarterScreenA), nullptr, true, false);
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::HalfScreenA));
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);
		gfxList.Enqueue<EndEvent>();

		// Blur
		gfxList.Enqueue<BeginEvent>("Blur Pass");
		gfxList.Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::GaussianH));
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::QuarterScreenB), nullptr, true, false);
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::QuarterScreenA));
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);

		gfxList.Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::GaussianV));
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::QuarterScreenA), nullptr, true, false);
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::QuarterScreenB));
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);

		gfxList.Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::GaussianH));
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::QuarterScreenB), nullptr, false, false);
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::QuarterScreenA));
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);

		gfxList.Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::GaussianV));
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::QuarterScreenA), nullptr, false, false);
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::QuarterScreenB));
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);
		gfxList.Enqueue<EndEvent>();

		// Upsample
		gfxList.Enqueue<BeginEvent>("Upsampling Pass");
		gfxList.Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::Resample));
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::HalfScreenA), nullptr, true, false);
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::QuarterScreenA));
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);

		gfxList.Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::Resample));
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::Luminance), nullptr, true, false);
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::HalfScreenA));
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);
		gfxList.Enqueue<EndEvent>();

		// Bloom
		gfxList.Enqueue<BeginEvent>("Bloom Pass");
		gfxList.Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::Bloom));
		gfxList.Enqueue<SetRenderTarget>(gfx.GetBackBuffer(), nullptr, true, false);
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::Luminance));
		gfxList.Enqueue<SetTextureResource>(31, gfx.GetIntermediateTexture(IntermediateTexture::LDR));
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);
		gfxList.Enqueue<ClearTextureResource>(31);
		gfxList.Enqueue<EndEvent>();
	}
}

void RenderAssembler::QueueDeferredLightPasses(SceneRenderData& aRenderData)
{
	GraphicsEngine& gfx = GraphicsEngine::Get();
	GraphicsCommandList& gfxList = gfx.GetGraphicsCommandList();

	// Directional Light
	{
		PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Directional Light");
		gfxList.Enqueue<BeginEvent>("Ambient & Directional Light Pass");
		gfxList.Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::DeferredDirectionalLight));
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::HDR), nullptr, false, false);
		gfxList.Enqueue<SetTextureResource>(126, aRenderData.ambientLight->GetCubemap());
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::Luminance));
		gfxList.Enqueue<SetGBufferAsResource>();
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);
		gfxList.Enqueue<ClearTextureResource>(126);
		gfxList.Enqueue<EndEvent>();
	}

	// Pointlights
	{
		PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Pointlights");
		gfxList.Enqueue<BeginEvent>("Pointlight Pass");
		gfxList.Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::DeferredPointlight));
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::HDR), nullptr, false, false);
		gfxList.Enqueue<SetGBufferAsResource>();
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<EndEvent>();
	}

	// Spotlights
	{
		PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Spotlights");
		gfxList.Enqueue<BeginEvent>("Spotlight Pass");
		gfxList.Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::DeferredSpotlight));
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::HDR), nullptr, false, false);
		gfxList.Enqueue<SetGBufferAsResource>();
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<EndEvent>();
	}
}

void RenderAssembler::QueueDeferredObjects(SceneRenderData& aRenderData)
{
	PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Queue All Deferred Objects");

	for (auto& gameObject : aRenderData.drawDeferred)
	{
		auto transform = gameObject->GetComponent<Transform>();

		if (auto model = gameObject->GetComponent<Model>())
		{
			if (!model->GetShouldViewcull() || IsInsideFrustum(aRenderData.mainCamera, transform, model->GetBoundingBox()))
			{
				UpdateBoundingBox(transform, model->GetBoundingBox());

				PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Create Mesh Data");

				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderMesh>(
					model->GetMesh(),
					model->GetMaterials(),
					transform->GetWorldMatrix(),
					model->GetCustomShaderData_1(),
					model->GetCustomShaderData_2());
			}
		}

		if (auto animModel = gameObject->GetComponent<AnimatedModel>())
		{
			if (!animModel->GetShouldViewcull() || IsInsideFrustum(aRenderData.mainCamera, transform, animModel->GetBoundingBox()))
			{
				UpdateBoundingBox(transform, animModel->GetBoundingBox());

				PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Create Anim Mesh Data");
				
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderAnimatedMesh>(
					animModel->GetMesh(),
					animModel->GetMaterials(),
					transform->GetWorldMatrix(),
					animModel->GetCurrentPose());
			}
		}

		if (auto instancedModel = gameObject->GetComponent<InstancedModel>())
		{
			if (!instancedModel->GetShouldViewcull() || IsInsideFrustum(aRenderData.mainCamera, transform, instancedModel->GetBoundingBox()))
			{
				UpdateBoundingBox(transform, instancedModel->GetBoundingBox());

				PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Create Instanced Mesh Data");
				
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderInstancedMesh>(
					instancedModel->GetMesh(),
					instancedModel->GetMaterials(),
					transform->GetWorldMatrix(),
					&instancedModel->GetInstanceBuffer(),
					instancedModel->GetMeshCount());
			}
		}
	}
}

void RenderAssembler::QueueForwardObjects(SceneRenderData& aRenderData)
{
	PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Queue All Forward Objects");

	for (auto& gameObject : aRenderData.drawForward)
	{
		auto transform = gameObject->GetComponent<Transform>();

		if (auto model = gameObject->GetComponent<Model>())
		{
			if (!model->GetShouldViewcull() || IsInsideFrustum(aRenderData.mainCamera, transform, model->GetBoundingBox()))
			{
				PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Create Mesh Data");

				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderMesh>(
					model->GetMesh(),
					model->GetMaterials(),
					transform->GetWorldMatrix(),
					model->GetCustomShaderData_1(),
					model->GetCustomShaderData_2());
			}
		}

		if (auto animModel = gameObject->GetComponent<AnimatedModel>())
		{
			if (!animModel->GetShouldViewcull() || IsInsideFrustum(aRenderData.mainCamera, transform, animModel->GetBoundingBox()))
			{
				PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Create Anim Mesh Data");

				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderAnimatedMesh>(
					animModel->GetMesh(),
					animModel->GetMaterials(),
					transform->GetWorldMatrix(),
					animModel->GetCurrentPose());
			}
		}

		if (auto instancedModel = gameObject->GetComponent<InstancedModel>())
		{
			if (!instancedModel->GetShouldViewcull() || IsInsideFrustum(aRenderData.mainCamera, transform, instancedModel->GetBoundingBox()))
			{
				PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Create Instanced Mesh Data");

				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderInstancedMesh>(
					instancedModel->GetMesh(),
					instancedModel->GetMaterials(),
					transform->GetWorldMatrix(),
					&instancedModel->GetInstanceBuffer(),
					instancedModel->GetMeshCount());
			}
		}
	}
}

void RenderAssembler::QueueShadowmapTextureResources(bool aUseStaticShadowmaps, SceneRenderData& aRenderData)
{
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetMarker>("Set Shadowmaps");

	if (!(aUseStaticShadowmaps && !aRenderData.directionalLight->gameObject->GetStatic()))
	{
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetTextureResource>(GraphicsSettings::DIRECTIONAL_LIGHT_SHADOWS_SLOT, aUseStaticShadowmaps ? aRenderData.directionalLight->GetStaticShadowMap() : aRenderData.directionalLight->GetDynamicShadowMap());
	}

	for (int i = 0; i < aRenderData.pointLights.size(); i++)
	{
		if (i >= MAX_POINTLIGHTS) break;
		if (aUseStaticShadowmaps && !aRenderData.pointLights[i]->gameObject->GetStatic()) continue;
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetTextureResource>(GraphicsSettings::POINTLIGHT_SHADOWS_START_SLOT + i, aUseStaticShadowmaps ? aRenderData.pointLights[i]->GetStaticShadowMap() : aRenderData.pointLights[i]->GetDynamicShadowMap());
	}

	for (int i = 0; i < aRenderData.spotLights.size(); i++)
	{
		if (i >= MAX_SPOTLIGHTS) break;
		if (aUseStaticShadowmaps && !aRenderData.spotLights[i]->gameObject->GetStatic()) continue;
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetTextureResource>(GraphicsSettings::SPOTLIGHT_SHADOWS_START_SLOT + i, aUseStaticShadowmaps ? aRenderData.spotLights[i]->GetStaticShadowMap() : aRenderData.spotLights[i]->GetDynamicShadowMap());
	}
}

void RenderAssembler::QueueUpdateLightBuffer(SceneRenderData& aRenderData)
{
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetMarker>("Update Light Buffer");

	LightBuffer lightBufferData;

	if (aRenderData.ambientLight)
	{
		lightBufferData.AmbientLight.Color = aRenderData.ambientLight->GetColor();
		lightBufferData.AmbientLight.Intensity = aRenderData.ambientLight->GetIntensity();
	}

	if (aRenderData.directionalLight)
	{
		if (auto dirLightCamComp = aRenderData.directionalLight->gameObject->GetComponent<Camera>())
		{
			lightBufferData.DirLight.Projection = dirLightCamComp->GetProjectionMatrix();
			lightBufferData.DirLight.FrustumSize = dirLightCamComp->GetViewportDimensions();
			lightBufferData.DirLight.NearPlane = dirLightCamComp->GetNearPlane();
		}

		if (auto dirLightTransformComp = aRenderData.directionalLight->gameObject->GetComponent<Transform>())
		{
			lightBufferData.DirLight.View = dirLightTransformComp->GetWorldMatrix().GetFastInverse();
		}

		lightBufferData.DirLight.Color = aRenderData.directionalLight->GetColor();
		lightBufferData.DirLight.Intensity = aRenderData.directionalLight->GetIntensity();
		lightBufferData.DirLight.Direction = aRenderData.directionalLight->GetDirection();

		lightBufferData.DirLight.CastShadows = aRenderData.directionalLight->CastsShadows();
		lightBufferData.DirLight.MinBias = aRenderData.directionalLight->GetMinShadowBias();
		lightBufferData.DirLight.MaxBias = aRenderData.directionalLight->GetMaxShadowBias();
		lightBufferData.DirLight.LightSize = aRenderData.directionalLight->GetLightSize();
	}

	int activePLights = 0;
	for (int i = 0; i < static_cast<int>(aRenderData.pointLights.size()); i++)
	{
		auto& pLight = aRenderData.pointLights[i];

		if (pLight && pLight->GetActive())
		{
			++activePLights;

			if (auto cam = pLight->gameObject->GetComponent<Camera>())
			{
				lightBufferData.PointLights[i].FrustumSize = cam->GetViewportDimensions();
				lightBufferData.PointLights[i].NearPlane = cam->GetNearPlane();
				lightBufferData.PointLights[i].Projection = cam->GetProjectionMatrix();
			}

			lightBufferData.PointLights[i].Position = pLight->GetPosition();
			lightBufferData.PointLights[i].Color = pLight->GetColor();
			lightBufferData.PointLights[i].Intensity = pLight->GetIntensity();
			lightBufferData.PointLights[i].CastShadows = pLight->CastsShadows();
			lightBufferData.PointLights[i].MinBias = pLight->GetMinShadowBias();
			lightBufferData.PointLights[i].MaxBias = pLight->GetMaxShadowBias();
			lightBufferData.PointLights[i].LightSize = pLight->GetLightSize();
		}
	}
	lightBufferData.NumPointLights = activePLights;

	int activeSLights = 0;
	for (int i = 0; i < static_cast<int>(aRenderData.spotLights.size()); i++)
	{
		auto& sLight = aRenderData.spotLights[i];

		if (sLight && sLight->GetActive())
		{
			++activeSLights;

			if (auto transform = sLight->gameObject->GetComponent<Transform>())
			{
				lightBufferData.SpotLights[i].View = transform->GetWorldMatrix().GetFastInverse();
			}

			if (auto cam = sLight->gameObject->GetComponent<Camera>())
			{
				lightBufferData.SpotLights[i].FrustumSize = cam->GetViewportDimensions();
				lightBufferData.SpotLights[i].NearPlane = cam->GetNearPlane();
				lightBufferData.SpotLights[i].Projection = cam->GetProjectionMatrix();
			}

			lightBufferData.SpotLights[i].Position = sLight->GetPosition();
			lightBufferData.SpotLights[i].Direction = sLight->GetDirection();
			lightBufferData.SpotLights[i].ConeAngle = sLight->GetConeAngleRadians();
			lightBufferData.SpotLights[i].Color = sLight->GetColor();
			lightBufferData.SpotLights[i].Intensity = sLight->GetIntensity();
			lightBufferData.SpotLights[i].CastShadows = sLight->CastsShadows();
			lightBufferData.SpotLights[i].MinBias = sLight->GetMinShadowBias();
			lightBufferData.SpotLights[i].MaxBias = sLight->GetMaxShadowBias();
			lightBufferData.SpotLights[i].LightSize = sLight->GetLightSize();
		}
	}
	lightBufferData.NumSpotLights = activeSLights;

	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateLightBuffer>(std::move(lightBufferData));
}

void RenderAssembler::QueueSpotLightShadows(SceneRenderData& aRenderData)
{
	PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Spotlight Shadows");

	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<BeginEvent>("Spotlight Shadows");

	for (int i = 0; i < aRenderData.spotLights.size(); i++)
	{
		std::shared_ptr<SpotLight> spotLight = aRenderData.spotLights[i];
		std::shared_ptr<Camera> lightCam = spotLight->gameObject->GetComponent<Camera>();
		std::shared_ptr<Transform> lightTransform = spotLight->gameObject->GetComponent<Transform>();

		FrameBuffer frameBuffer;
		frameBuffer.InvView = lightTransform->GetWorldMatrix().GetFastInverse();
		frameBuffer.Projection = lightCam->GetProjectionMatrix();
		frameBuffer.ViewPosition = Math::ToVector4(lightTransform->GetTranslation(true), 1.0f);
		frameBuffer.ViewDirection = Math::ToVector4(lightTransform->GetForwardVector(true), 1.0f);
		frameBuffer.NearPlane = lightCam->GetNearPlane();
		frameBuffer.FarPlane = lightCam->GetFarPlane();
		frameBuffer.Time = { static_cast<float>(Engine::Get().GetTimer().GetTimeSinceProgramStart()), Engine::Get().GetTimer().GetDeltaTime() };
		frameBuffer.Resolution = Engine::Get().GetResolution();
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(std::move(frameBuffer));

		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::Shadow));
		if (spotLight->gameObject->GetStatic())
		{
			if (myShouldUpdateStaticShadows)
			{
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, spotLight->GetStaticShadowMap(), false, true);
				QueueObjectShadows(aRenderData.castShadowsStatic, lightCam);
			}

			GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, spotLight->GetDynamicShadowMap(), false, true);
			QueueObjectShadows(aRenderData.castShadowsDynamic, lightCam);
		}
		else
		{
			GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, spotLight->GetDynamicShadowMap(), false, true);
			QueueObjectShadows(aRenderData.castShadowsStatic, lightCam);
			QueueObjectShadows(aRenderData.castShadowsDynamic, lightCam);
		}
	}

	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<EndEvent>();
}

void RenderAssembler::QueuePointLightShadows(SceneRenderData& aRenderData)
{
	PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Pointlight Shadows");

	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<BeginEvent>("Pointlight Shadows");

	for (int i = 0; i < aRenderData.pointLights.size(); i++)
	{
		std::shared_ptr<PointLight> pointLight = aRenderData.pointLights[i];
		std::shared_ptr<Camera> lightCam = pointLight->gameObject->GetComponent<Camera>();
		std::shared_ptr<Transform> lightTransform = pointLight->gameObject->GetComponent<Transform>();

		FrameBuffer frameBuffer;
		frameBuffer.InvView = lightTransform->GetWorldMatrix().GetFastInverse();
		frameBuffer.Projection = lightCam->GetProjectionMatrix();
		frameBuffer.ViewPosition = Math::ToVector4(lightTransform->GetTranslation(true), 1.0f);
		frameBuffer.ViewDirection = Math::ToVector4(lightTransform->GetForwardVector(true), 1.0f);
		frameBuffer.NearPlane = lightCam->GetNearPlane();
		frameBuffer.FarPlane = lightCam->GetFarPlane();
		frameBuffer.Time = { static_cast<float>(Engine::Get().GetTimer().GetTimeSinceProgramStart()), Engine::Get().GetTimer().GetDeltaTime() };
		frameBuffer.Resolution = Engine::Get().GetResolution();
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(std::move(frameBuffer));

		UpdateShadowBuffer::ShadowData shadowData;
		shadowData.cameraTransform = pointLight->gameObject->GetComponent<Transform>()->GetMatrix();
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateShadowBuffer>(std::move(shadowData));

		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::ShadowCube));
		if (pointLight->gameObject->GetStatic())
		{
			if (myShouldUpdateStaticShadows)
			{
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, pointLight->GetStaticShadowMap(), false, true);
				QueueObjectShadows(aRenderData.castShadowsStatic, pointLight);
			}

			GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, pointLight->GetDynamicShadowMap(), false, true);
			QueueObjectShadows(aRenderData.castShadowsDynamic, pointLight);
		}
		else
		{
			GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, pointLight->GetDynamicShadowMap(), false, true);
			QueueObjectShadows(aRenderData.castShadowsStatic, pointLight);
			QueueObjectShadows(aRenderData.castShadowsDynamic, pointLight);
		}
	}

	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<EndEvent>();
}

void RenderAssembler::QueueDirectionalLightShadows(SceneRenderData& aRenderData)
{
	PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Directional Light Shadows");

	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<BeginEvent>("Directional Light Shadows");

	std::shared_ptr<DirectionalLight> dLight = aRenderData.directionalLight;

	if (!dLight->CastsShadows()) return;

	std::shared_ptr<Camera> lightCam = dLight->gameObject->GetComponent<Camera>();
	std::shared_ptr<Transform> lightTransform = dLight->gameObject->GetComponent<Transform>();

	FrameBuffer frameBuffer;
	frameBuffer.InvView = lightTransform->GetWorldMatrix().GetFastInverse();
	frameBuffer.Projection = lightCam->GetProjectionMatrix();
	frameBuffer.ViewPosition = Math::ToVector4(lightTransform->GetTranslation(true), 1.0f);
	frameBuffer.ViewDirection = Math::ToVector4(lightTransform->GetForwardVector(true), 1.0f);
	frameBuffer.NearPlane = lightCam->GetNearPlane();
	frameBuffer.FarPlane = lightCam->GetFarPlane();
	frameBuffer.Time = { static_cast<float>(Engine::Get().GetTimer().GetTimeSinceProgramStart()), Engine::Get().GetTimer().GetDeltaTime() };
	frameBuffer.Resolution = Engine::Get().GetResolution();
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(std::move(frameBuffer));

	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::Shadow));
	if (dLight->gameObject->GetStatic())
	{
		if (myShouldUpdateStaticShadows)
		{
			GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, dLight->GetStaticShadowMap(), false, true);
			QueueObjectShadows(aRenderData.castShadowsStatic, lightCam);
		}

		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, dLight->GetDynamicShadowMap(), false, true);
		QueueObjectShadows(aRenderData.castShadowsDynamic, lightCam);
	}
	else
	{
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, dLight->GetDynamicShadowMap(), false, true);
		QueueObjectShadows(aRenderData.castShadowsStatic, lightCam);
		QueueObjectShadows(aRenderData.castShadowsDynamic, lightCam);
	}
	
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<EndEvent>();
}

void RenderAssembler::QueueObjectShadows(const std::vector<std::shared_ptr<GameObject>>& aGameObjects, std::shared_ptr<Camera> aRenderCamera)
{
	PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Queue Object Shadows");

	for (auto& object : aGameObjects)
	{
		auto transform = object->GetComponent<Transform>();

		if (auto model = object->GetComponent<Model>())
		{
			if (!model->GetShouldViewcull() || IsInsideFrustum(aRenderCamera.get(), transform, model->GetBoundingBox()))
			{
				PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Create Mesh Shadow Data");

				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderMeshShadow>(
					model->GetMesh(),
					transform->GetWorldMatrix());
			}
		}

		if (auto animModel = object->GetComponent<AnimatedModel>())
		{
			if (!animModel->GetShouldViewcull() || IsInsideFrustum(aRenderCamera.get(), transform, animModel->GetBoundingBox()))
			{
				PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Create Anim Mesh Shadow Data");

				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderAnimatedMeshShadow>(
					animModel->GetMesh(),
					transform->GetWorldMatrix(),
					animModel->GetCurrentPose());
			}
		}

		if (auto instancedModel = object->GetComponent<InstancedModel>())
		{
			if (!instancedModel->GetShouldViewcull() || IsInsideFrustum(aRenderCamera.get(), transform, instancedModel->GetBoundingBox()))
			{
				PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Create Instanced Mesh Shadow Data");

				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderInstancedMeshShadow>(
					instancedModel->GetMesh(),
					transform->GetWorldMatrix(),
					&instancedModel->GetInstanceBuffer(),
					instancedModel->GetMeshCount());
			}
		}
	}
}

void RenderAssembler::QueueObjectShadows(const std::vector<std::shared_ptr<GameObject>>& aGameObjects, std::shared_ptr<PointLight> aPointLight)
{
	PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Queue Object Shadows");

	for (auto& object : aGameObjects)
	{
		auto transform = object->GetComponent<Transform>();

		if (auto model = object->GetComponent<Model>())
		{
			if (!model->GetShouldViewcull() || IsInsideRadius(aPointLight, transform, model->GetBoundingBox()))
			{
				PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Create Mesh Shadow Data");

				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderMeshShadow>(
					model->GetMesh(), 
					transform->GetWorldMatrix());
			}
		}

		if (auto animModel = object->GetComponent<AnimatedModel>())
		{
			if (!animModel->GetShouldViewcull() || IsInsideRadius(aPointLight, transform, animModel->GetBoundingBox()))
			{
				PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Create Anim Mesh Shadow Data");

				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderAnimatedMeshShadow>(
					animModel->GetMesh(),
					transform->GetWorldMatrix(),
					animModel->GetCurrentPose());
			}
		}

		if (auto instancedModel = object->GetComponent<InstancedModel>())
		{
			if (!instancedModel->GetShouldViewcull() || IsInsideRadius(aPointLight, transform, instancedModel->GetBoundingBox()))
			{
				PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Create Instanced Mesh Shadow Data");

				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderInstancedMeshShadow>(
					instancedModel->GetMesh(),
					transform->GetWorldMatrix(),
					&instancedModel->GetInstanceBuffer(),
					instancedModel->GetMeshCount());
			}
		}
	}
}

void RenderAssembler::QueueObjectsDebug(SceneRenderData& aRenderData)
{
	for (auto& gameObject : aRenderData.drawForward)
	{
		auto transform = gameObject->GetComponent<Transform>();

		if (auto model = gameObject->GetComponent<Model>())
		{
			if (!model->GetShouldViewcull() || IsInsideFrustum(aRenderData.mainCamera, transform, model->GetBoundingBox()))
			{
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderMeshDebugPass>(
					model->GetMesh(), 
					model->GetMaterials(), 
					transform->GetWorldMatrix(), 
					model->GetCustomShaderData_1(), 
					model->GetCustomShaderData_2());
			}
		}

		if (auto animModel = gameObject->GetComponent<AnimatedModel>())
		{
			if (!animModel->GetShouldViewcull() || IsInsideFrustum(aRenderData.mainCamera, transform, animModel->GetBoundingBox()))
			{
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderAnimatedMeshDebugPass>(
					animModel->GetMesh(), 
					animModel->GetMaterials(), 
					transform->GetWorldMatrix(), 
					animModel->GetCurrentPose());
			}
		}

		if (auto instancedModel = gameObject->GetComponent<InstancedModel>())
		{
			if (!instancedModel->GetShouldViewcull() || IsInsideFrustum(aRenderData.mainCamera, transform, instancedModel->GetBoundingBox()))
			{
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderInstancedMeshDebugPass>(
					instancedModel->GetMesh(), 
					instancedModel->GetMaterials(), 
					transform->GetWorldMatrix(),
					&instancedModel->GetInstanceBuffer(),
					instancedModel->GetMeshCount());
			}
		}
	}

	for (auto& gameObject : aRenderData.drawDeferred)
	{
		auto transform = gameObject->GetComponent<Transform>();

		if (auto model = gameObject->GetComponent<Model>())
		{
			if (!model->GetShouldViewcull() || IsInsideFrustum(aRenderData.mainCamera, transform, model->GetBoundingBox()))
			{
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderMeshDebugPass>(
					model->GetMesh(),
					model->GetMaterials(),
					transform->GetWorldMatrix(),
					model->GetCustomShaderData_1(),
					model->GetCustomShaderData_2());
			}
		}

		if (auto animModel = gameObject->GetComponent<AnimatedModel>())
		{
			if (!animModel->GetShouldViewcull() || IsInsideFrustum(aRenderData.mainCamera, transform, animModel->GetBoundingBox()))
			{
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderAnimatedMeshDebugPass>(
					animModel->GetMesh(),
					animModel->GetMaterials(),
					transform->GetWorldMatrix(),
					animModel->GetCurrentPose());
			}
		}

		if (auto instancedModel = gameObject->GetComponent<InstancedModel>())
		{
			if (!instancedModel->GetShouldViewcull() || IsInsideFrustum(aRenderData.mainCamera, transform, instancedModel->GetBoundingBox()))
			{
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderInstancedMeshDebugPass>(
					instancedModel->GetMesh(),
					instancedModel->GetMaterials(),
					transform->GetWorldMatrix(),
					&instancedModel->GetInstanceBuffer(),
					instancedModel->GetMeshCount());
			}
		}
	}
}

void RenderAssembler::QueueDebugLines(SceneRenderData& aRenderData)
{
	PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Debug Lines");

	if (Engine::Get().DrawBoundingBoxes)
	{
		Engine::Get().GetDebugDrawer().DrawBoundingBox(aRenderData.sceneBoundingBox, Math::Matrix4x4f(), Math::Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
		Engine::Get().GetDebugDrawer().DrawBoundingBox(myVisibleObjectsBB, Math::Matrix4x4f(), Math::Vector4f(1.0f, 0.0f, 1.0f, 1.0f));

		for (auto& gameObject : aRenderData.drawBoundingBoxesObjects)
		{
			if (auto model = gameObject->GetComponent<Model>())
			{
				Engine::Get().GetDebugDrawer().DrawBoundingBox(model);
			}
			
			if (auto animModel = gameObject->GetComponent<AnimatedModel>())
			{
				Engine::Get().GetDebugDrawer().DrawBoundingBox(animModel);
			}
			
			if (auto instancedModel = gameObject->GetComponent<InstancedModel>())
			{
				Engine::Get().GetDebugDrawer().DrawBoundingBox(instancedModel);
			}
		}
	}

	if (Engine::Get().DrawColliders)
	{
		Math::Vector4f colorGreen = { 0, 1.0f, 0, 1.0f };
		Math::Vector4f colorRed = { 1.0f, 0, 0, 1.0f };

		for (auto& gameObject : aRenderData.drawCollidersObjects)
		{
			if (auto boxCollider = gameObject->GetComponent<BoxCollider>())
			{
				Engine::Get().GetDebugDrawer().DrawBoundingBox(boxCollider->GetAABB(), gameObject->GetComponent<Transform>()->GetWorldMatrix(), boxCollider->IsOverlapping() ? colorRed : colorGreen);
			}

			if (auto sphereCollider = gameObject->GetComponent<SphereCollider>())
			{
				Engine::Get().GetDebugDrawer().DrawBoundingSphere(sphereCollider->GetSphere(), gameObject->GetComponent<Transform>()->GetWorldMatrix(), sphereCollider->IsOverlapping() ? colorRed : colorGreen);
			}
		}
	}

	if (Engine::Get().DrawCameraFrustums)
	{
		for (auto& gameObject : aRenderData.drawCameraFrustumsObjects)
		{
			if (auto cam = gameObject->GetComponent<Camera>())
			{
				Engine::Get().GetDebugDrawer().DrawCameraFrustum(cam);
			}
		}
	}
}

bool RenderAssembler::IsInsideFrustum(Camera* aRenderCamera, std::shared_ptr<Transform> aObjectTransform, const Math::AABB3D<float>& aObjectAABB)
{
	PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Is Inside Frustum");
	if (!Engine::Get().UseViewCulling) return true;

	return aRenderCamera->GetViewcullingIntersection(aObjectTransform, aObjectAABB);
}

bool RenderAssembler::IsInsideFrustum(Camera* aRenderCamera, std::shared_ptr<Transform> aObjectTransform, const std::array<Math::Vector3f, 8>& aLightFrustum)
{
	if (!Engine::Get().UseViewCulling) return true;

	return aRenderCamera->GetViewcullingIntersection(aObjectTransform, aLightFrustum);
}

bool RenderAssembler::IsInsideRadius(std::shared_ptr<PointLight> aPointLight, std::shared_ptr<Transform> aObjectTransform, const Math::AABB3D<float>& aObjectAABB)
{
	PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Is Inside Radius");
	if (!Engine::Get().UseViewCulling) return true;

	std::shared_ptr<Transform> pointLightTransform = aPointLight->gameObject->GetComponent<Transform>();
	std::shared_ptr<Camera> pointLightCam = aPointLight->gameObject->GetComponent<Camera>();
	if (!pointLightTransform) return true;

	Math::AABB3D<float> lightAABB(Math::Vector3f(), pointLightCam->GetFarPlane(), pointLightCam->GetFarPlane(), pointLightCam->GetFarPlane());
	lightAABB = lightAABB.GetAABBinNewSpace(pointLightTransform->GetWorldMatrix() * aObjectTransform->GetWorldMatrixInverse());
	return Math::IntersectionBetweenAABBS(lightAABB, aObjectAABB);

	//Math::Sphere<float> sphere(Math::Vector3f(), pointLightCam->GetFarPlane());
	//sphere = sphere.GetSphereinNewSpace(pointLightTransform->GetWorldMatrix() * aObjectTransform->GetWorldMatrixInverse());
	//return Math::IntersectionSphereAABB(sphere, aObjectAABB);
}

void RenderAssembler::UpdateBoundingBox(std::shared_ptr<Transform> aTransform, const Math::AABB3D<float>& aBoundingBox)
{
	PIXScopedEvent(PIX_COLOR_INDEX(6), "RenderAssembler Update Bounding Box");

	Math::Vector3f bbMin = myVisibleObjectsBB.GetMin();
	Math::Vector3f bbMax = myVisibleObjectsBB.GetMax();
	auto& corners = aBoundingBox.GetCorners();

	for (Math::Vector3f corner : corners)
	{
		corner = Math::ToVector3(Math::ToVector4(corner, 1.0f) * aTransform->GetWorldMatrix());

		bbMin.x = std::fminf(corner.x, bbMin.x);
		bbMax.x = std::fmaxf(corner.x, bbMax.x);
		bbMin.y = std::fminf(corner.y, bbMin.y);
		bbMax.y = std::fmaxf(corner.y, bbMax.y);
		bbMin.z = std::fminf(corner.z, bbMin.z);
		bbMax.z = std::fmaxf(corner.z, bbMax.z);
	}

	myVisibleObjectsBB.InitWithMinAndMax(bbMin, bbMax);
}

// TEMP
void RenderAssembler::Init()
{
	//myTestSprite = std::make_shared<Sprite>();
	//myTestSprite->SetTexture(GraphicsEngine::Get().GetPerlinNoiseTexture());
	//myTestSprite->SetPosition(Math::Vector2f(500.0f, 500.0f));
	//myTestSprite->SetSize(Math::Vector2f(600.0f, 600.0f));
}

void RenderAssembler::DrawTestUI()
{
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(GraphicsEngine::Get().GetPSO(PSOType::Sprite));
	//GraphicsEngine::Get().ChangePipelineState(GraphicsEngine::Get().GetPSO(PSOType::Spritesheet));
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderSprite>(myTestSprite->GetTexture(), myTestSprite->GetMatrix());
}