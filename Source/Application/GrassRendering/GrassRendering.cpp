#include "Enginepch.h"
#include "GrassRendering.h"
#include <Application/AppSettings.h>
#include <GameEngine/Engine.h>
#include <GameEngine/Time/Timer.h>
#include <future>

#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/InstancedModel.h"
#include "AssetTypes/MeshAsset.h"
#include "AssetTypes/MaterialAsset.h"

#include "CommonUtilities/Random.hpp"
#include "CommonUtilities/PerlinNoise.h"

Application* CreateApplication()
{
	AppSettings::LoadSettings(std::filesystem::current_path() / APP_SETTINGS_PATH);
    return new GrassRendering();
}

void GrassRendering::InitializeApplication()
{
	//Engine::Get().DrawColliders = true;
	//Engine::Get().DrawBoundingBoxes = true;

	Engine::Get().GetInputHandler().RegisterBinaryAction("F6", Keys::F6, GenericInput::ActionType::Clicked);
	Engine::Get().GetInputHandler().RegisterBinaryAction("QUIT", Keys::ESCAPE, GenericInput::ActionType::Clicked);

	auto& sh = Engine::Get().GetSceneHandler();
	sh.LoadScene("Scenes/GrassRendering.SCENE");

	for (int i = myChunkRangeMin; i < myChunkRangeMax; i++)
	{
		for (int j = myChunkRangeMin; j < myChunkRangeMax; j++)
		{
			std::shared_ptr<GameObject> instancedModelObj = std::make_shared<GameObject>();
			Math::Vector3f currentChunkOffset(i * myChunkOffset, 0.0f, j * myChunkOffset);
			instancedModelObj->AddComponent<Transform>(currentChunkOffset);
			std::shared_ptr<InstancedModel> instancedModel = instancedModelObj->AddComponent<InstancedModel>(myInstanceRows * myInstanceRows);
			instancedModel->SetMesh(AssetManager::Get().GetAsset<MeshAsset>("SM_GrassBlade.fbx")->mesh);
			instancedModel->SetMaterialOnSlot(0, AssetManager::Get().GetAsset<MaterialAsset>("SimpleGrass.mat")->material);
			instancedModel->SetCastShadows(false);

			std::vector<std::future<Math::Matrix4x4f>> futureTransforms;

			for (int outer = 0; outer < myInstanceRows; outer++)
			{
				for (int inner = 0; inner < myInstanceRows; inner++)
				{
					futureTransforms.emplace_back(std::async(std::launch::async, &GrassRendering::CalculateInstanceTransform, this, inner, outer, i, j, currentChunkOffset));
				}
			}

			for (auto& futureTransform : futureTransforms)
			{
				futureTransform.wait();

				Math::Matrix4x4f transform = futureTransform.get();
				if (transform(4, 2) < 1.0f)
					instancedModel->AddInstanceNoBufferUpdate(transform);
			}

			instancedModel->UpdateInstanceBuffer();
			Engine::Get().GetSceneHandler().Instantiate(instancedModelObj);
		}
	}
}

void GrassRendering::UpdateApplication()
{
	if (Engine::Get().GetInputHandler().GetBinaryAction("QUIT"))
		QuitApplication();

	if (Engine::Get().GetInputHandler().GetBinaryAction("F6"))
	{
		currentDebugMode += 1;
		if (currentDebugMode >= static_cast<unsigned>(DebugRenderMode::COUNT))
		{
			currentDebugMode = 0;
		}

		GraphicsEngine::Get().CurrentDebugRenderMode = static_cast<DebugRenderMode>(currentDebugMode);
	}
}

void GrassRendering::UpdateDebug()
{
	Math::Vector2f resolution = Engine::Get().GetResolution();
	ImGui::SetNextWindowPos({ 0.0f, 0.0f });
	ImGui::SetNextWindowContentSize({ 0.16f * resolution.x, 0.16f * resolution.y });
	if (ImGui::Begin("Performance Info", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
	{
		if (ImGui::BeginTable("PerformanceTable", 2, 0, { 0.18f * resolution.x, 0 }))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.CellPadding = { 0.001f * resolution.x, 0.004f * resolution.y };

			// FPS
			{
				Math::Vector4f color = { 1.0f, 1.0f, 1.0f, 1.0f };
				int fps = Engine::Get().GetTimer().GetAverageFPS();
				if (fps < 60) color = { 1.0f, 1.0f, 0.0f, 1.0f };
				if (fps < 30) color = { 1.0f, 0.0f, 0.0f, 1.0f };

				ImGui::TableNextColumn();
				ImGui::Text("FPS:");
				ImGui::TableNextColumn();
				ImGui::TextColored({ color.x, color.y, color.z, color.w }, std::to_string(fps).c_str());
				ImGui::TableNextColumn();

				ImGui::Text("Frametime (ms):");
				ImGui::TableNextColumn();
				ImGui::Text("%.2f", Engine::Get().GetTimer().GetAverageFrameTimeMS());
				ImGui::TableNextColumn();
			}

			// Draw calls
			{
				ImGui::Text("Drawcalls:");
				ImGui::TableNextColumn();
				ImGui::Text(std::to_string(GraphicsEngine::Get().GetDrawcallAmount()).c_str());
				ImGui::TableNextColumn();
			}

			// Scene objects
			{
				ImGui::Text("Scene Objects:");
				ImGui::TableNextColumn();
				ImGui::Text(std::string(std::to_string(Engine::Get().GetSceneHandler().GetObjectAmount())).c_str());

				ImGui::TableNextColumn();

				ImGui::Text("Active Scene Objects:");
				ImGui::TableNextColumn();
				ImGui::Text(std::to_string(Engine::Get().GetSceneHandler().GetActiveObjectAmount()).c_str());
				ImGui::TableNextColumn();
			}
			ImGui::EndTable();
		}
	}
	ImGui::End();
}

Math::Matrix4x4f GrassRendering::CalculateInstanceTransform(int aInner, int aOuter, int aChunkHorizontalIndex, int aChunkVerticalIndex, Math::Vector3f aCurrentChunkOffset) const
{
	Math::Matrix4x4f instanceMatrix = Math::Matrix4x4f::CreateRollPitchYawMatrix({ 0.0f, Utilities::RandomInRange(-180.0f, 180.0f), 0.0f });

	instanceMatrix(4, 1) = Utilities::RandomVariation(aInner * myInstanceOffset, myInstanceOffsetVariation);
	instanceMatrix(4, 3) = Utilities::RandomVariation(aOuter * myInstanceOffset, myInstanceOffsetVariation);

	Math::Vector2f coordinates;
	coordinates.x = 2000.0f + aChunkHorizontalIndex * myChunkOffset + instanceMatrix(4, 1);
	coordinates.y = 2000.0f + aChunkVerticalIndex * myChunkOffset + instanceMatrix(4, 3);
	Math::Vector2f mapSize(myChunkOffset, myChunkOffset);
	float noise = SamplePerlin(coordinates, mapSize, 24, 5.0f);
	float randomVariation = Utilities::RandomVariation(1.0f, 0.2f);
	float scale = Math::Lerp(1.0f - mySizeVariation, 1.0f + mySizeVariation, (1.0f + noise + randomVariation) * 0.3f);
	instanceMatrix(1, 1) = scale;
	instanceMatrix(2, 2) = scale;
	instanceMatrix(3, 3) = scale;

	Math::Vector3f hitPoint;
	Engine::Get().GetSceneHandler().Raycast({ aCurrentChunkOffset.x + instanceMatrix(4, 1), aCurrentChunkOffset.y + myYRayOrigin, aCurrentChunkOffset.z + instanceMatrix(4, 3) }, { 0.0f, -1.0f, 0.0f }, hitPoint);
	instanceMatrix(4, 2) = hitPoint.y;

	return instanceMatrix;
}
