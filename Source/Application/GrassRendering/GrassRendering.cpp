#include "Enginepch.h"
#include "GrassRendering.h"
#include <Application/AppSettings.h>
#include <GameEngine/Engine.h>
#include <GameEngine/Time/Timer.h>

#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/InstancedModel.h"
#include "AssetTypes/MeshAsset.h"
#include "AssetTypes/MaterialAsset.h"

#include "CommonUtilities/Random.hpp"

Application* CreateApplication()
{
	AppSettings::LoadSettings(std::filesystem::current_path() / APP_SETTINGS_PATH);
    return new GrassRendering();
}

void GrassRendering::InitializeApplication()
{
	Engine::Get().GetInputHandler().RegisterBinaryAction("F6", Keys::F6, GenericInput::ActionType::Clicked);

	auto& sh = Engine::Get().GetSceneHandler();
	sh.LoadScene("Scenes/GrassRendering.SCENE");

	std::shared_ptr<GameObject> instancedModelObj = std::make_shared<GameObject>();
	instancedModelObj->AddComponent<Transform>(Math::Vector3f(-3000.0f, -10.0f, -3000.0f));
	std::shared_ptr<InstancedModel> instancedModel = instancedModelObj->AddComponent<InstancedModel>(40000);
	instancedModel->SetMesh(AssetManager::Get().GetAsset<MeshAsset>("SM_Grass.fbx")->mesh);
	instancedModel->SetMaterialOnSlot(0, AssetManager::Get().GetAsset<MaterialAsset>("Grass.mat")->material);

	float defaultOffset = 40.0f;
	float offsetVariation = 40.0f;

	float minSize = 0.5f;
	float maxSize = 2.0f;

	for (int outer = 0; outer < 200; outer++)
	{
		for (int inner = 0; inner < 200; inner++)
		{
			Math::Matrix4x4f instanceMatrix = Math::Matrix4x4f::CreateRollPitchYawMatrix({ 0.0f, Utilities::RandomInRange(-180.0f, 180.0f), 0.0f });
			instanceMatrix(4, 1) = Utilities::RandomVariation(inner * defaultOffset, offsetVariation);
			instanceMatrix(4, 3) = Utilities::RandomVariation(outer * defaultOffset, offsetVariation);

			float scale = Utilities::RandomInRange(minSize, maxSize);
			instanceMatrix(1, 1) = scale;
			instanceMatrix(2, 2) = scale;
			instanceMatrix(3, 3) = scale;
			instancedModel->AddInstance(instanceMatrix);
		}
	}

	Engine::Get().GetSceneHandler().Instantiate(instancedModelObj);
}

void GrassRendering::UpdateApplication()
{
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
				int fps = Engine::Get().GetTimer().GetFPS();
				if (fps < 60) color = { 1.0f, 1.0f, 0.0f, 1.0f };
				if (fps < 30) color = { 1.0f, 0.0f, 0.0f, 1.0f };

				ImGui::TableNextColumn();
				ImGui::Text("FPS:");
				ImGui::TableNextColumn();
				ImGui::TextColored({ color.x, color.y, color.z, color.w }, std::to_string(fps).c_str());
				ImGui::TableNextColumn();

				ImGui::Text("Frametime (ms):");
				ImGui::TableNextColumn();
				ImGui::Text("%.2f", Engine::Get().GetTimer().GetFrameTimeMS());
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