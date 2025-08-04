#include "Enginepch.h"
#include "FeatureShowcase.h"
#include <Application/AppSettings.h>
#include <Engine.h>
#include <Time/Timer.h>
#include <Psapi.h>

#include <ComponentSystem/GameObject.h>
#include <ComponentSystem/Components/Transform.h>
#include <ComponentSystem/Components/Lights/AmbientLight.h>
#include <ComponentSystem/Components/Lights/DirectionalLight.h>
#include <ComponentSystem/Components/Lights/PointLight.h>
#include <ComponentSystem/Components/Lights/SpotLight.h>
#include <ComponentSystem/Components/Graphics/Model.h>
#include <ComponentSystem/Components/Graphics/AnimatedModel.h>
#include <ComponentSystem/Components/Graphics/InstancedModel.h>
#include <ComponentSystem/Components/Graphics/VFXModel.h>
#include <ComponentSystem/Components/Physics/Colliders/BoxCollider.h>
#include <ComponentSystem/Components/Physics/Colliders/SphereCollider.h>

#include <ComponentSystem/Components/Graphics/ParticleSystem.h>

#include "AssetTypes/MeshAsset.h"
#include "AssetTypes/AnimationAsset.h"
#include "AssetTypes/MaterialAsset.h"
#include <ComponentSystem/Components/Physics/Rigidbody.h>
#include <ComponentSystem/Components/Movement/PhysicsController.h>
#include "CommonUtilities/Random.hpp"

static int sRamUsage = 0;
static int sRamUsageChange = 0;
static float sTimeSinceLastMemoryCheck = 0.0f;
static float sMemoryCheckTimeInterval = 1.0f;

Application* CreateApplication()
{
	AppSettings::LoadSettings(std::filesystem::current_path() / APP_SETTINGS_PATH);
    return new FeatureShowcase();
}

void FeatureShowcase::InitializeApplication()
{
	Engine::Get().GetInputHandler().SetControllerDeadZone(0.1f, 0.06f);
	Engine::Get().GetAudioEngine().LoadBank("AudioBanks/Master.bank");
	Engine::Get().GetAudioEngine().LoadBank("AudioBanks/Master.strings.bank");
	Engine::Get().GetAudioEngine().LoadBank("AudioBanks/Test.bank");

	Engine::Get().GetAudioEngine().AddBus(BusType::Music, "Music");
	Engine::Get().GetAudioEngine().AddBus(BusType::Ambience, "Ambience");
	Engine::Get().GetAudioEngine().AddBus(BusType::SFX, "SFX");

	Engine::Get().GetSceneHandler().LoadScene("Scenes/FeatureShowCaseScene.SCENE");

	InputHandler& inputHandler = Engine::Get().GetInputHandler();
	inputHandler.RegisterBinaryAction("W", Keys::W, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("A", Keys::A, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("S", Keys::S, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("D", Keys::D, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("LMB", Keys::MOUSELBUTTON, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("RMB", Keys::MOUSERBUTTON, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("SPACE", Keys::SPACE, GenericInput::ActionType::Clicked);
	inputHandler.RegisterBinaryAction("F6", Keys::F6, GenericInput::ActionType::Clicked);
	inputHandler.RegisterBinaryAction("F7", Keys::F7, GenericInput::ActionType::Clicked);
	inputHandler.RegisterAnalog2DAction("MousePos", MouseMovement2D::MousePos);
	inputHandler.RegisterAnalog2DAction("MouseNDCPos", MouseMovement2D::MousePosNDC);
	inputHandler.RegisterAnalog2DAction("MouseDelta", MouseMovement2D::MousePosDelta);

	inputHandler.RegisterBinaryAction("A_Gamepad", ControllerButtons::A, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("B", ControllerButtons::B, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("X", ControllerButtons::X, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("Y", ControllerButtons::Y, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("Down", ControllerButtons::DPAD_DOWN, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("Left", ControllerButtons::DPAD_LEFT, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("Up", ControllerButtons::DPAD_UP, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("Right", ControllerButtons::DPAD_RIGHT, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("LS", ControllerButtons::LEFT_STICK_DOWN, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("RS", ControllerButtons::RIGHT_STICK_DOWN, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("LB", ControllerButtons::LEFT_SHOULDER, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("RB", ControllerButtons::RIGHT_SHOULDER, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("Start", ControllerButtons::START, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("Back", ControllerButtons::BACK, GenericInput::ActionType::Held);
	inputHandler.RegisterAnalog2DAction("LeftStick", ControllerAnalog2D::LEFT_STICK);
	inputHandler.RegisterAnalog2DAction("RightStick", ControllerAnalog2D::RIGHT_STICK);
	inputHandler.RegisterAnalogAction("LeftTrigger", ControllerAnalog::LEFT_TRIGGER);
	inputHandler.RegisterAnalogAction("RightTrigger", ControllerAnalog::RIGHT_TRIGGER);

	inputHandler.RegisterBinaryAction("SharedAction", Keys::W, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("SharedAction", ControllerButtons::A, GenericInput::ActionType::Held);

	std::shared_ptr<GameObject> instancedModelObj = std::make_shared<GameObject>();
	instancedModelObj->AddComponent<Transform>(Math::Vector3f(-500.0f, 0, 1500.0f));
	std::shared_ptr<InstancedModel> instancedModel = instancedModelObj->AddComponent<InstancedModel>();
	instancedModel->SetMesh(AssetManager::Get().GetAsset<MeshAsset>("SM_Chest.fbx")->mesh);
	instancedModel->SetMaterialOnSlot(0, AssetManager::Get().GetAsset<MaterialAsset>("Chest.mat")->material);

	for (int outer = 0; outer < 10; outer++)
	{
		for (int inner = 0; inner < 10; inner++)
		{
			Math::Matrix4x4f instanceMatrix;
			instanceMatrix(4, 1) = inner * 250.0f;
			instanceMatrix(4, 3) = outer * 250.0f;
			instancedModel->AddInstance(instanceMatrix);
		}
	}

	Engine::Get().GetSceneHandler().Instantiate(instancedModelObj);

	Engine::Get().DrawColliders = true;
}

void FeatureShowcase::UpdateApplication()
{
	if (Engine::Get().GetInputHandler().GetBinaryAction("SPACE") && !Engine::Get().GetInputHandler().GetBinaryAction("RMB"))
	{
		currentAnimation += 1;
		if (currentAnimation >= animationNames.size())
		{
			currentAnimation = 0;
		}

		std::shared_ptr<AnimatedModel> tgaBroModel = Engine::Get().GetSceneHandler().FindGameObjectByName("TgaBro")->GetComponent<AnimatedModel>();
		tgaBroModel->SetCurrentAnimationOnLayer(animationNames[currentAnimation], "", 0.5f);
	}

	if (Engine::Get().GetInputHandler().GetBinaryAction("F6"))
	{
		currentDebugMode += 1;
		if (currentDebugMode >= static_cast<unsigned>(DebugRenderMode::COUNT))
		{
			currentDebugMode = 0;
		}

		GraphicsEngine::Get().CurrentDebugRenderMode = static_cast<DebugRenderMode>(currentDebugMode);
	}

	if (Engine::Get().GetInputHandler().GetBinaryAction("F7"))
	{
		GraphicsEngine::Get().GetPostProcessingSettings().SSAOEnabled = !GraphicsEngine::Get().GetPostProcessingSettings().SSAOEnabled;
	}
}

void FeatureShowcase::UpdateDebug()
{
	TopMenuBar();

	if (myShowFeatureOptions)
		FeatureOptions();
	
	if (myShowLightingSettings)
		LightingSettings();
	
	if (myShowPerformanceInfo)
		PerformanceInfo();
	
	if (myShowControlsInfo)
		ControlsInfo();

	if (myShowResolutionOptions)
		ResolutionOptions();

	if (myShowPhysicsPlayground)
		PhysicsPlayground();
}

void FeatureShowcase::TopMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::MenuItem("General"))
			myShowFeatureOptions = !myShowFeatureOptions;

		if (ImGui::MenuItem("Lighting"))
			myShowLightingSettings = !myShowLightingSettings;

		if (ImGui::MenuItem("Performance Info"))
			myShowPerformanceInfo = !myShowPerformanceInfo;

		if (ImGui::MenuItem("Controls Info"))
			myShowControlsInfo = !myShowControlsInfo;

		if (ImGui::MenuItem("Resolution Options"))
			myShowResolutionOptions = !myShowResolutionOptions;

		if (ImGui::MenuItem("Physics Playground"))
			myShowPhysicsPlayground = !myShowPhysicsPlayground;

		ImGui::EndMainMenuBar();
	}
}

void FeatureShowcase::FeatureOptions()
{
	Math::Vector2f resolution = Engine::Get().GetResolution();
	ImGui::SetNextWindowPos({ 0.01f * resolution.x, 0.02f * resolution.y });
	ImGui::SetNextWindowContentSize({ 0.16f * resolution.x, 0.35f * resolution.y });
	if (ImGui::Begin("FeatureShowcase", &myShowFeatureOptions))
	{
		ImGui::Checkbox("Use Viewculling", &Engine::Get().UseViewCulling);
		ImGui::Checkbox("Draw Bounding Boxes", &Engine::Get().DrawBoundingBoxes);
		ImGui::Checkbox("Draw Camera Frustums", &Engine::Get().DrawCameraFrustums);
		ImGui::Checkbox("Draw Colliders", &Engine::Get().DrawColliders);

		

		PostProcessingSettings& ppSettings = GraphicsEngine::Get().GetPostProcessingSettings();

		// Rendering
		{
			ImGui::Text("Rendering Mode");
			if (ImGui::BeginCombo("##RenderModeDropdown", DebugRenderModeName(static_cast<DebugRenderMode>(GraphicsEngine::Get().CurrentDebugRenderMode))))
			{
				for (unsigned i = 0; i < static_cast<unsigned>(DebugRenderMode::COUNT); i++)
				{
					if (ImGui::Selectable(DebugRenderModeName(static_cast<DebugRenderMode>(i))))
						GraphicsEngine::Get().CurrentDebugRenderMode = static_cast<DebugRenderMode>(i);
				}
				ImGui::EndCombo();
			}
		}

		// Tonemapping
		{
			ImGui::Text("Tonemapper");
			if (ImGui::BeginCombo("##TonemapperDropdown", TonemapperName(ppSettings.Tonemapper)))
			{
				for (unsigned i = 0; i < static_cast<unsigned>(TonemapperType::COUNT); i++)
				{
					if (ImGui::Selectable(TonemapperName(static_cast<TonemapperType>(i)))) ppSettings.Tonemapper = static_cast<TonemapperType>(i);
				}
				ImGui::EndCombo();
			}
		}

		ImGui::Checkbox("Enable Bloom", &ppSettings.BloomEnabled);
		ImGui::Checkbox("Enable SSAO", &ppSettings.SSAOEnabled);

		ImGui::Separator();

		// Animation
		{
			ImGui::Text("Animations");
			std::shared_ptr<AnimatedModel> tgaBroModel = Engine::Get().GetSceneHandler().FindGameObjectByName("TgaBro")->GetComponent<AnimatedModel>();
			if (ImGui::BeginCombo("##AnimationDropdown", tgaBroModel->GetCurrentAnimationNameOnLayer(0).c_str()))
			{
				if (ImGui::Selectable("Idle")) tgaBroModel->SetCurrentAnimationOnLayer("Idle", "", 0.5f);
				if (ImGui::Selectable("Walk")) tgaBroModel->SetCurrentAnimationOnLayer("Walk", "", 0.5f);;
				if (ImGui::Selectable("Run")) tgaBroModel->SetCurrentAnimationOnLayer("Run", "", 0.5f);;
				if (ImGui::Selectable("Wave")) tgaBroModel->SetCurrentAnimationOnLayer("Wave", "", 0.5f);;
				ImGui::EndCombo();
			}
		}

		// Test Audio
		{
			float sfxVolume = Engine::Get().GetAudioEngine().GetVolumeOfBus(BusType::SFX);
			if (ImGui::SliderFloat("SFX Volume", &sfxVolume, 0, 1.0f)) Engine::Get().GetAudioEngine().SetVolumeOfBus(BusType::SFX, sfxVolume);

			float musicVolume = Engine::Get().GetAudioEngine().GetVolumeOfBus(BusType::Music);
			if (ImGui::SliderFloat("Music Volume", &musicVolume, 0, 1.0f)) Engine::Get().GetAudioEngine().SetVolumeOfBus(BusType::Music, musicVolume);
		}

		// Test VFX
		{
			std::shared_ptr<VFXModel> vfx = Engine::Get().GetSceneHandler().FindGameObjectByName("VFXTest")->GetComponent<VFXModel>();
			if (ImGui::Button("Play VFX"))
			{
				if (vfx)
				{
					vfx->PlayVFX();
				}
			}
		}
	}
	ImGui::End();
}

void FeatureShowcase::LightingSettings()
{
	Math::Vector2f resolution = Engine::Get().GetResolution();
	ImGui::SetNextWindowPos({ 0.01f * resolution.x, 0.4f * resolution.y });
	ImGui::SetNextWindowContentSize({ 0.16f * resolution.x, 0.55f * resolution.y });

	PostProcessingSettings& ppSettings = GraphicsEngine::Get().GetPostProcessingSettings();

	ImGui::Begin("Lighting Settings", &myShowLightingSettings);
	{
		if (ImGui::BeginTabBar("Lights"))
		{
			if (ImGui::BeginTabItem("Ambient Light"))
			{
				std::shared_ptr<GameObject> ambientLight = Engine::Get().GetSceneHandler().FindGameObjectByName("A_Light");
				bool active = ambientLight->GetActive();
				if (ImGui::Checkbox("Set Active##Ambient", &active)) ambientLight->SetActive(active);

				std::shared_ptr<AmbientLight> aLight = ambientLight->GetComponent<AmbientLight>();
				float intensity = aLight->GetIntensity();
				ImGui::Text("Intensity");
				if (ImGui::SliderFloat("##AmbientIntensity", &intensity, 0, 10.0f, "%.3f", ImGuiSliderFlags_Logarithmic)) aLight->SetIntensity(intensity);
				ImGui::Spacing();
				float color[3] = { aLight->GetColor().x, aLight->GetColor().y, aLight->GetColor().z };
				ImGui::Text("Color");
				unsigned flags = ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHex | ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_NoSidePreview;
				if (ImGui::ColorPicker3("##AmbientColor", color, flags)) aLight->SetColor({ color[0], color[1], color[2] });

				// Luminance
				ImGui::Text("Luminance");
				if (ImGui::BeginCombo("##LuminanceDropdown", LuminanceName(ppSettings.LuminanceFunction)))
				{
					for (unsigned i = 0; i < static_cast<unsigned>(LuminanceType::COUNT); i++)
					{
						if (ImGui::Selectable(LuminanceName(static_cast<LuminanceType>(i)))) ppSettings.LuminanceFunction = static_cast<LuminanceType>(i);
					}

					ImGui::EndCombo();
				}

				// Bloom
				ImGui::Text("Bloom");
				if (ImGui::BeginCombo("##BloomDropdown", BloomName(ppSettings.BloomFunction)))
				{
					for (unsigned i = 0; i < static_cast<unsigned>(BloomType::COUNT); i++)
					{
						if (ImGui::Selectable(BloomName(static_cast<BloomType>(i)))) ppSettings.BloomFunction = static_cast<BloomType>(i);
					}

					ImGui::EndCombo();
				}

				ImGui::SliderFloat("Bloom Intensity", &ppSettings.BloomStrength, 0, 1.0f);
				ImGui::SliderFloat("SSAO Noise Power", &ppSettings.SSAONoisePower, 0, 1.0f);
				ImGui::SliderFloat("SSAO Radius", &ppSettings.SSAORadius, 0, 0.5f);
				ImGui::SliderFloat("SSAO Bias", &ppSettings.SSAOBias, 0, 0.1f);
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Directional Light"))
			{
				std::shared_ptr<GameObject> directionalLight = Engine::Get().GetSceneHandler().FindGameObjectByName("D_Light");
				bool active = directionalLight->GetActive();
				if (ImGui::Checkbox("Set Active##Directional", &active)) directionalLight->SetActive(active);

				std::shared_ptr<DirectionalLight> dLight = directionalLight->GetComponent<DirectionalLight>();
				float intensity = dLight->GetIntensity();
				ImGui::Text("Intensity");
				if (ImGui::SliderFloat("##DirectionalIntensity", &intensity, 0, 10.0f, "%.3f", ImGuiSliderFlags_Logarithmic)) dLight->SetIntensity(intensity);
				ImGui::Spacing();
				float minBias = dLight->GetMinShadowBias();
				float maxBias = dLight->GetMaxShadowBias();
				float lightSize = dLight->GetLightSize();
				ImGui::Text("Shadow Min Bias");
				if (ImGui::SliderFloat("##DirectionalMinBias", &minBias, 0, 0.001f, "%.5f")) dLight->SetShadowBias(minBias, maxBias);
				ImGui::Text("Shadow Max Bias");
				if (ImGui::SliderFloat("##DirectionalMaxBias", &maxBias, 0, 0.1f, "%.5f")) dLight->SetShadowBias(minBias, maxBias);
				ImGui::Text("Light Size");
				if (ImGui::SliderFloat("##DirectionalSize", &lightSize, 0, 2000.0f)) dLight->SetLightSize(lightSize);
				ImGui::Spacing();
				float color[3] = { dLight->GetColor().x, dLight->GetColor().y, dLight->GetColor().z };
				ImGui::Text("Color");
				unsigned flags = ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHex | ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_NoSidePreview;
				if (ImGui::ColorPicker3("##DirectionalColor", color, flags)) dLight->SetColor({ color[0], color[1], color[2] });
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Pointlight"))
			{
				std::shared_ptr<GameObject> pointLight = Engine::Get().GetSceneHandler().FindGameObjectByName("P_Light");
				bool active = pointLight->GetActive();
				if (ImGui::Checkbox("Set Active##Point", &active))  pointLight->SetActive(active);

				std::shared_ptr<PointLight> pLight = pointLight->GetComponent<PointLight>();
				float value = pLight->GetIntensity();
				ImGui::Text("Intensity");
				if (ImGui::SliderFloat("##PointIntensity", &value, 0, 500000.0f, "%.3f", ImGuiSliderFlags_Logarithmic)) pLight->SetIntensity(value);

				ImGui::Spacing();

				float minBias = pLight->GetMinShadowBias();
				float maxBias = pLight->GetMaxShadowBias();
				float lightSize = pLight->GetLightSize();
				ImGui::Text("Shadow Min Bias");
				if (ImGui::SliderFloat("##PointMinBias", &minBias, 0, 0.001f, "%.5f")) pLight->SetShadowBias(minBias, maxBias);
				ImGui::Text("Shadow Max Bias");
				if (ImGui::SliderFloat("##PointMaxBias", &maxBias, 0, 0.01f, "%.5f")) pLight->SetShadowBias(minBias, maxBias);
				ImGui::Text("Light Size");
				if (ImGui::SliderFloat("##PointSize", &lightSize, 0, 10000.0f)) pLight->SetLightSize(lightSize);
				ImGui::Spacing();

				float color[3] = { pLight->GetColor().x, pLight->GetColor().y, pLight->GetColor().z };
				ImGui::Text("Color");
				unsigned flags = ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHex | ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_NoSidePreview;
				if (ImGui::ColorPicker3("##PointColor", color, flags)) pLight->SetColor({ color[0], color[1], color[2] });

				ImGui::Spacing();

				float pos[3] = { pLight->GetPosition().x, pLight->GetPosition().y, pLight->GetPosition().z };
				ImGui::Text("Position");
				if (ImGui::DragFloat3("##PLightPos", pos)) pLight->gameObject->GetComponent<Transform>()->SetTranslation(pos[0], pos[1], pos[2]);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Spotlight"))
			{
				std::shared_ptr<GameObject> spotLight = Engine::Get().GetSceneHandler().FindGameObjectByName("S_Light");
				bool active = spotLight->GetActive();
				if (ImGui::Checkbox("Set Active##Spot", &active)) spotLight->SetActive(active);

				std::shared_ptr<SpotLight> sLight = spotLight->GetComponent<SpotLight>();
				float intensity = sLight->GetIntensity();
				ImGui::Text("Intensity");
				if (ImGui::SliderFloat("##SpotIntensity", &intensity, 0, 1000000.0f, "%.3f", ImGuiSliderFlags_Logarithmic)) sLight->SetIntensity(intensity);
				ImGui::Spacing();
				float angle = sLight->GetConeAngleDegrees();
				ImGui::Text("Focus");
				if (ImGui::SliderFloat("##SpotConeAngle", &angle, 600.0f, 50000.0f, "%.3f", ImGuiSliderFlags_Logarithmic)) sLight->SetConeAngle(angle);
				ImGui::Spacing();

				float minBias = sLight->GetMinShadowBias();
				float maxBias = sLight->GetMaxShadowBias();
				float lightSize = sLight->GetLightSize();
				ImGui::Text("Shadow Min Bias");
				if (ImGui::SliderFloat("##SpotMinBias", &minBias, 0, 0.001f, "%.5f")) sLight->SetShadowBias(minBias, maxBias);
				ImGui::Text("Shadow Max Bias");
				if (ImGui::SliderFloat("##SpotMaxBias", &maxBias, 0, 0.1f, "%.5f")) sLight->SetShadowBias(minBias, maxBias);
				ImGui::Text("Light Size");
				if (ImGui::SliderFloat("##SpotSize", &lightSize, 0, 10.0f)) sLight->SetLightSize(lightSize);
				ImGui::Spacing();

				float color[3] = { sLight->GetColor().x, sLight->GetColor().y, sLight->GetColor().z };
				ImGui::Text("Color");
				unsigned flags = ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHex | ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_NoSidePreview;
				if (ImGui::ColorPicker3("##SpotColor", color, flags)) sLight->SetColor({ color[0], color[1], color[2] });

				ImGui::Spacing();

				float pos[3] = { sLight->GetPosition().x, sLight->GetPosition().y, sLight->GetPosition().z };
				ImGui::Text("Position");
				if (ImGui::DragFloat3("##SLightPos", pos)) sLight->gameObject->GetComponent<Transform>()->SetTranslation(pos[0], pos[1], pos[2]);
				Math::Vector3f rotation = sLight->gameObject->GetComponent<Transform>()->GetRotation();
				float rot[3] = { rotation.x, rotation.y, rotation.z };
				ImGui::Text("Rotation");
				if (ImGui::DragFloat3("##SLightRot", rot)) sLight->gameObject->GetComponent<Transform>()->SetRotation(rot[0], rot[1], rot[2]);
				
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}

void FeatureShowcase::PerformanceInfo()
{
	Math::Vector2f resolution = Engine::Get().GetResolution();
	ImGui::SetNextWindowPos({ 0.18f * resolution.x, 0.02f * resolution.y });
	ImGui::SetNextWindowContentSize({ 0.16f * resolution.x, 0.2f * resolution.y });
	if (ImGui::Begin("Performance Info", &myShowPerformanceInfo))
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

			ImGui::Spacing();

			// Memory Usage
			{
				sTimeSinceLastMemoryCheck += Engine::Get().GetTimer().GetDeltaTime();

				sTimeSinceLastMemoryCheck;
				if (sTimeSinceLastMemoryCheck > sMemoryCheckTimeInterval)
				{
					sTimeSinceLastMemoryCheck = 0;

					HANDLE hProcess = {};
					PROCESS_MEMORY_COUNTERS pmc;
					hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());

					if (hProcess != NULL)
					{
						if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
						{
							int newRamUsage = static_cast<int>(pmc.WorkingSetSize / (1024.0 * 1024.0));
							sRamUsageChange = newRamUsage - sRamUsage;
							sRamUsage = newRamUsage;
						}

						CloseHandle(hProcess);
					}
				}

				Math::Vector4f color = { 1.0f, 0.0f, 0.0f, 1.0f };
				if (sRamUsageChange <= 0) color = { 0.0f, 1.0f, 0.0f, 1.0f };

				ImGui::Text("RAM used:");
				ImGui::TableNextColumn();
				ImGui::Text(std::string(std::to_string(sRamUsage) + " Mb").c_str());

				ImGui::TableNextColumn();

				ImGui::Text("RAM usage fluctuation:");
				ImGui::TableNextColumn();
				ImGui::TextColored({ color.x, color.y, color.z, color.w }, std::string(std::to_string(sRamUsageChange) + " Mb").c_str());
				ImGui::TableNextColumn();
			}
			ImGui::EndTable();
		}
	}
	ImGui::End();
}

void FeatureShowcase::ControlsInfo()
{
	Math::Vector2f resolution = Engine::Get().GetResolution();
	ImGui::SetNextWindowPos({ 0.8f * resolution.x, 0.02f * resolution.y });
	ImGui::SetNextWindowContentSize({ 0.15f * resolution.x, 0.26f * resolution.y });
	if (ImGui::Begin("Controller Info", &myShowControlsInfo))
	{
		InputHandler& cont = Engine::Get().GetInputHandler();

		ImGui::Text("Left Stick: ");
		ImGui::SameLine();
		ImGui::Text("%.2f", cont.GetAnalogAction2D("LeftStick").x);
		ImGui::SameLine();
		ImGui::Text(", ");
		ImGui::SameLine();
		ImGui::Text("%.2f", cont.GetAnalogAction2D("LeftStick").y);

		ImGui::Text("Right Stick: ");
		ImGui::SameLine();
		ImGui::Text("%.2f", cont.GetAnalogAction2D("RightStick").x);
		ImGui::SameLine();
		ImGui::Text(", ");
		ImGui::SameLine();
		ImGui::Text("%.2f", cont.GetAnalogAction2D("RightStick").y);

		ImGui::Text("Left Trigger: ");
		ImGui::SameLine();
		ImGui::Text("%.2f", cont.GetAnalogAction("LeftTrigger"));

		ImGui::Text("Right Trigger: ");
		ImGui::SameLine();
		ImGui::Text("%.2f", cont.GetAnalogAction("RightTrigger"));


		ImVec4 red = { 1.0f, 0, 0, 1.0f };
		ImVec4 green = { 0, 1.0f, 0, 1.0f };
		ImVec4 color = (cont.GetBinaryAction("A_Gamepad") ? green : red);
		ImGui::TextColored(color, "A");
		ImGui::SameLine();
		color = (cont.GetBinaryAction("B") ? green : red);
		ImGui::TextColored(color, "B");
		ImGui::SameLine();
		color = (cont.GetBinaryAction("X") ? green : red);
		ImGui::TextColored(color, "X");
		ImGui::SameLine();
		color = (cont.GetBinaryAction("Y") ? green : red);
		ImGui::TextColored(color, "Y");

		color = (cont.GetBinaryAction("LB") ? green : red);
		ImGui::TextColored(color, "LB");
		ImGui::SameLine();
		color = (cont.GetBinaryAction("RB") ? green : red);
		ImGui::TextColored(color, "RB");
		ImGui::SameLine();
		color = (cont.GetBinaryAction("LS") ? green : red);
		ImGui::TextColored(color, "LS");
		ImGui::SameLine();
		color = (cont.GetBinaryAction("RS") ? green : red);
		ImGui::TextColored(color, "RS");

		color = (cont.GetBinaryAction("Left") ? green : red);
		ImGui::TextColored(color, "Left");
		ImGui::SameLine();
		color = (cont.GetBinaryAction("Up") ? green : red);
		ImGui::TextColored(color, "Up");
		ImGui::SameLine();
		color = (cont.GetBinaryAction("Right") ? green : red);
		ImGui::TextColored(color, "Right");
		ImGui::SameLine();
		color = (cont.GetBinaryAction("Down") ? green : red);
		ImGui::TextColored(color, "Down");

		color = (cont.GetBinaryAction("Start") ? green : red);
		ImGui::TextColored(color, "Start");
		ImGui::SameLine();
		color = (cont.GetBinaryAction("Back") ? green : red);
		ImGui::TextColored(color, "Back");

		ImGui::Separator();

		ImGui::Text("Mouse Pos: ");
		ImGui::SameLine();
		ImGui::Text("%.2f", cont.GetAnalogAction2D("MousePos").x);
		ImGui::SameLine();
		ImGui::Text(", ");
		ImGui::SameLine();
		ImGui::Text("%.2f", cont.GetAnalogAction2D("MousePos").y);

		ImGui::Text("NDC Mouse Pos: ");
		ImGui::SameLine();
		ImGui::Text("%.2f", cont.GetAnalogAction2D("MouseNDCPos").x);
		ImGui::SameLine();
		ImGui::Text(", ");
		ImGui::SameLine();
		ImGui::Text("%.2f", cont.GetAnalogAction2D("MouseNDCPos").y);

		ImGui::Text("Mouse Delta: ");
		ImGui::SameLine();
		ImGui::Text("%.2f", cont.GetAnalogAction2D("MouseDelta").x);
		ImGui::SameLine();
		ImGui::Text(", ");
		ImGui::SameLine();
		ImGui::Text("%.2f", cont.GetAnalogAction2D("MouseDelta").y);

		color = (cont.GetBinaryAction("W") ? green : red);
		ImGui::TextColored(color, "W");
		ImGui::SameLine();
		color = (cont.GetBinaryAction("A") ? green : red);
		ImGui::TextColored(color, "A");
		ImGui::SameLine();
		color = (cont.GetBinaryAction("S") ? green : red);
		ImGui::TextColored(color, "S");
		ImGui::SameLine();
		color = (cont.GetBinaryAction("D") ? green : red);
		ImGui::TextColored(color, "D");

		color = (cont.GetBinaryAction("LMB") ? green : red);
		ImGui::TextColored(color, "LMB");
		ImGui::SameLine();
		color = (cont.GetBinaryAction("RMB") ? green : red);
		ImGui::TextColored(color, "RMB");

		color = (cont.GetBinaryAction("SharedAction") ? green : red);
		ImGui::TextColored(color, "SharedAction");
	}
	ImGui::End();
}

void FeatureShowcase::ResolutionOptions()
{
	Math::Vector2f resolution = Engine::Get().GetResolution();
	ImGui::SetNextWindowPos({ 0.8f * resolution.x, 0.32f * resolution.y });
	ImGui::SetNextWindowContentSize({ 0.15f * resolution.x, 0.24f * resolution.y });
	if (ImGui::Begin("Resolution", &myShowResolutionOptions))
	{
		if (ImGui::Button("Set Maximized"))
		{
			GraphicsEngine::Get().MaximizeWindowSize();
		}

		if (ImGui::Button("1280 x 720"))
		{
			GraphicsEngine::Get().SetResolution(1280.0f, 720.0f);
			Engine::Get().SetResolution(1280.0f, 720.0f);
		}
		if (ImGui::Button("1600 x 900"))
		{
			GraphicsEngine::Get().SetResolution(1600.0f, 900.0f);
			Engine::Get().SetResolution(1600.0f, 900.0f);
		}
		if (ImGui::Button("1920 x 1080"))
		{
			GraphicsEngine::Get().SetResolution(1920.0f, 1080.0f);
			Engine::Get().SetResolution(1920.0f, 1080.0f);
		}
		if (ImGui::Button("2560 x 1440"))
		{
			GraphicsEngine::Get().SetResolution(2560.0f, 1440.0f);
			Engine::Get().SetResolution(2560.0f, 1440.0f);
		}
		if (ImGui::Button("3840 x 2160"))
		{
			GraphicsEngine::Get().SetResolution(3840.0f, 2160.0f);
			Engine::Get().SetResolution(3840.0f, 2160.0f);
		}
	}
	ImGui::End();
}

void FeatureShowcase::PhysicsPlayground()
{
	Math::Vector2f resolution = Engine::Get().GetResolution();
	ImGui::SetNextWindowPos({ 0.8f * resolution.x, 0.32f * resolution.y });
	ImGui::SetNextWindowContentSize({ 0.15f * resolution.x, 0.24f * resolution.y });
	if (ImGui::Begin("Physics Playground", &myShowResolutionOptions))
	{
		if (ImGui::Button("Spawn Physics Cube"))
		{
			std::shared_ptr<GameObject> physicsObject = std::make_shared<GameObject>();
			auto transform = physicsObject->AddComponent<Transform>();
			transform->SetTranslation(2000.0f, 1000.0f, Utilities::RandomInRange<float>(-100.0f, 100.0f));
			transform->SetUniformScale(50.0f);
			physicsObject->AddComponent<Rigidbody>(1.0f);
			physicsObject->AddComponent<Model>(GraphicsEngine::Get().GetCubePrimitive());
			physicsObject->AddComponent<BoxCollider>(false, Math::Vector3f(2.0f, 2.0f, 2.0f));
			Engine::Get().GetSceneHandler().Instantiate(physicsObject);
		}

		if (ImGui::Button("Spawn Physics Sphere"))
		{
			std::shared_ptr<GameObject> physicsObject = std::make_shared<GameObject>();
			auto transform = physicsObject->AddComponent<Transform>();
			transform->SetTranslation(2000.0f, 1000.0f, Utilities::RandomInRange<float>(-100.0f, 100.0f));
			transform->SetUniformScale(50.0f);
			physicsObject->AddComponent<Rigidbody>(1.0f);
			physicsObject->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("sm_sphere.fbx")->mesh);
			physicsObject->AddComponent<SphereCollider>(false, 1.0f);
			Engine::Get().GetSceneHandler().Instantiate(physicsObject);
		}

		ImGui::BeginDisabled(myControllablePhysicsObject != nullptr);
		if (ImGui::Button("Spawn Controllable Physics Object"))
		{
			if (!myControllablePhysicsObject)
			{
				myControllablePhysicsObject = std::make_shared<GameObject>();
				auto transform = myControllablePhysicsObject->AddComponent<Transform>();
				transform->SetTranslation(2000.0f, 1000.0f, -200.0f);
				transform->SetUniformScale(50.0f);
				myControllablePhysicsObject->AddComponent<Rigidbody>(1.0f);
				myControllablePhysicsObject->AddComponent<Model>(GraphicsEngine::Get().GetCubePrimitive());
				myControllablePhysicsObject->AddComponent<BoxCollider>(false, Math::Vector3f(2.0f, 2.0f, 2.0f));
				myControllablePhysicsObject->AddComponent<PhysicsController>(1000.0f, 1.0f);
				Engine::Get().GetSceneHandler().Instantiate(myControllablePhysicsObject);
			}
		}
		ImGui::EndDisabled();

		ImGui::BeginDisabled(myControllablePhysicsObject == nullptr);
		if (ImGui::Button("Toggle Physics Object Controls"))
		{
			if (myControllablePhysicsObject)
			{
				auto cont = myControllablePhysicsObject->GetComponent<PhysicsController>();
				cont->SetActive(!cont->GetActive());
			}
		}
		ImGui::EndDisabled();
	}
	ImGui::End();
}
