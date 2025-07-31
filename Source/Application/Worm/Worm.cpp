#include "Enginepch.h"
#include "Worm.h"
#include <Application/AppSettings.h>
#include <GameEngine/Engine.h>
#include <GameEngine/ComponentSystem/GameObject.h>
#include <GameEngine/ComponentSystem/Components/Transform.h>
#include <GameEngine/ComponentSystem/Components/Graphics/Camera.h>
#include <GameEngine/ComponentSystem/Components/Graphics/Model.h>
#include <GameEngine/ComponentSystem/Components/Lights/AmbientLight.h>
#include <GameEngine/ComponentSystem/Components/Lights/DirectionalLight.h>

#include <AssetManager/AssetManager.h>
#include <GameEngine/AssetTypes/TextureAsset.h>
#include <GameEngine/AssetTypes/MaterialAsset.h>
#include <GameEngine/AssetTypes/MeshAsset.h>

#include <GameEngine/Time/Timer.h>

#undef min
#undef max

Application* CreateApplication()
{
	AppSettings::LoadSettings(std::filesystem::current_path() / APP_SETTINGS_PATH);
    return new Worm();
}

void Worm::InitializeApplication()
{
	InputHandler& inputHandler = Engine::Get().GetInputHandler();
	inputHandler.RegisterAnalog2DAction("MousePosNDC", MouseMovement2D::MousePosNDC);

	auto& sceneHandler = Engine::Get().GetSceneHandler();
	sceneHandler.LoadScene("WormGame/WormGame.SCENE");

	myBackgroundPlane = sceneHandler.FindGameObjectByName("Plane")->GetComponent<Transform>();
	myBackgroundPlaneMaterial = myBackgroundPlane->gameObject->GetComponent<Model>()->GetMaterialOnSlot(0);

	myCameraViewportDimensions = Camera::GetMainCamera()->GetViewportDimensions();

	for (int i = 0; i < max_pieces; i++)
	{
		std::shared_ptr<GameObject> newPiece = std::make_shared<GameObject>();
		sceneHandler.Instantiate(newPiece);
		myPlayerPieces[i] = newPiece->AddComponent<Transform>(Math::Vector3f(0.0f, i * -myBodyOffset, 0.0f), Math::Vector3f(-90.0f, 0.0f, 0.0f));
		myRotationHistory[i] = myPlayerPieces[i]->GetRotation().z;
		myPositionHistory[i] = myPlayerPieces[i]->GetTranslation();

		if (i == 0)
		{
			newPiece->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("SM_WormHead.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("WormHead.MAT")->material);
			myPlayerPieces[i]->AddTranslation(0.0f, 0.0f, -100.0f);
		}
		else if (i == max_pieces - 1)
			newPiece->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("SM_WormTail.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("WormTail.MAT")->material);
		else
			newPiece->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("SM_WormBody.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("WormBody.MAT")->material);
	}
}

void Worm::UpdateApplication()
{
	ScrollBackgroundPlane();
	UpdatePlayerHeadRotation();
	MovePlayerHead();
	
	float t = myDistanceTravelled / myBodyOffset;
	UpdatePlayerBodyPosition(t);
	if (t >= 1.0f)
	{
		myDistanceTravelled = 0.0f;
		++historyUpdated;
		UpdateRotationHistory(myPlayerPieces[0]->GetRotation().z);
		UpdatePositionHistory(myPlayerPieces[0]->GetTranslation());
	}
}

void Worm::UpdateDebug()
{
	if (ImGui::Begin("Position history"))
	{
		ImGui::Text("History updated %i times", historyUpdated);
		ImGui::Text("Highest Point: %f", myHighestYPoint);
		ImGui::Text("T: %f", myDistanceTravelled / myBodyOffset);

		ImGui::Spacing();

		for (size_t i = 0; i < max_pieces; i++)
		{
			Math::Vector3f pos = myPlayerPieces[i]->GetTranslation();
			ImGui::Text("%i: %f, %f", i, pos.x, pos.y);
		}
	}
	ImGui::End();
}

void Worm::ScrollBackgroundPlane()
{
	myBackgroundPlaneMaterial->MaterialSettings().uvOffset.y -= myBackgroundScrollSpeed;
}

void Worm::MovePlayerHead()
{
	myPlayerPieces[0]->AddTranslation(myPlayerPieces[0]->GetForwardVector() * myWormSpeed * Engine::Get().GetTimer().GetDeltaTime());
	myDistanceTravelled += myWormSpeed * Engine::Get().GetTimer().GetDeltaTime();

	if (myPlayerPieces[0]->GetTranslation().y > myHighestYPoint)
	{
		myHighestYPoint = myPlayerPieces[0]->GetTranslation().y;
	}
}

void Worm::UpdatePlayerHeadRotation()
{
	InputHandler& input = Engine::Get().GetInputHandler();
	Math::Vector2f mousePosNDC = input.GetAnalogAction2D("MousePosNDC");
	Math::Vector2f adjustedPos = mousePosNDC * myCameraViewportDimensions + Math::ToVector2(Camera::GetMainCamera()->gameObject->GetComponent<Transform>()->GetTranslation());

	Math::Vector2f diff = adjustedPos - Math::ToVector2(myPlayerPieces[0]->GetTranslation());
	diff = diff.GetNormalized();
	float newZRot = std::atan2f(-diff.x, diff.y) * Math::RADIANS_TO_DEGREES;
	myPlayerPieces[0]->SetRotation({ myPlayerPieces[0]->GetRotation().x, myPlayerPieces[0]->GetRotation().y, newZRot });
}

void Worm::UpdatePlayerBodyPosition(const float aT)
{
	for (int i = max_pieces - 1; i > 0; i--)
	{
		auto& currentPiece = myPlayerPieces[i];

		Math::Vector3f newPos = Math::Vector3f::Lerp(myPositionHistory[i], myPositionHistory[i - 1], aT);
		float zPos = currentPiece->GetTranslation().z;
		currentPiece->SetTranslation(newPos.x, newPos.y, zPos);
		float zAngle = Math::LerpAngle(myRotationHistory[i], myRotationHistory[i - 1], aT);
		currentPiece->SetRotation({ currentPiece->GetRotation().x, currentPiece->GetRotation().y, zAngle });
	}
}

void Worm::UpdateRotationHistory(const float aNewRotation)
{
	for (int i = max_pieces - 1; i > 0; i--)
	{
		myRotationHistory[i] = myRotationHistory[i - 1];
	}

	myRotationHistory[0] = aNewRotation;
}

void Worm::UpdatePositionHistory(const Math::Vector3f& aNewPosition)
{
	for (int i = max_pieces - 1; i > 0; i--)
	{
		myPositionHistory[i].x = myPositionHistory[i - 1].x;
		myPositionHistory[i].y = myPositionHistory[i - 1].y;
	}

	myPositionHistory[0] = aNewPosition;
}
