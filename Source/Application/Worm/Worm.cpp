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
#include <GameEngine/ComponentSystem/Components/Physics/Colliders/BoxCollider.h>

#include <AssetManager/AssetManager.h>
#include <GameEngine/AssetTypes/TextureAsset.h>
#include <GameEngine/AssetTypes/MaterialAsset.h>
#include <GameEngine/AssetTypes/MeshAsset.h>

#include <GameEngine/Time/Timer.h>
#include "CommonUtilities/Random.hpp"

#undef min
#undef max

Application* CreateApplication()
{
	AppSettings::LoadSettings(std::filesystem::current_path() / APP_SETTINGS_PATH);
    return new Worm();
}

void Worm::InitializeApplication()
{
	Engine::Get().DrawColliders = true;

	InputHandler& inputHandler = Engine::Get().GetInputHandler();
	inputHandler.RegisterAnalog2DAction("MousePosNDC", MouseMovement2D::MousePosNDC);

	auto& sceneHandler = Engine::Get().GetSceneHandler();
	sceneHandler.LoadScene("WormGame/WormGame.SCENE");

	myBackgroundPlane1 = sceneHandler.FindGameObjectByName("Plane1")->GetComponent<Transform>();
	myBackgroundPlane2 = sceneHandler.FindGameObjectByName("Plane2")->GetComponent<Transform>();

	myCameraViewportDimensions = Camera::GetMainCamera()->GetViewportDimensions();

	CreateWorm();
	CreateObstacles();
}

void Worm::UpdateApplication()
{
	ScrollBackgroundPlane();
	MoveObstacles();

	myTimeSinceLastObstacleSpawned += Engine::Get().GetTimer().GetDeltaTime();
	if (myTimeSinceLastObstacleSpawned > myObstacleSpawnTimer)
	{
		SpawnObstacle();
		myTimeSinceLastObstacleSpawned = 0.0f;
	}


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

void Worm::CreateWorm()
{
	auto& sceneHandler = Engine::Get().GetSceneHandler();

	for (int i = 0; i < max_pieces; i++)
	{
		std::shared_ptr<GameObject> newPiece = std::make_shared<GameObject>();
		sceneHandler.Instantiate(newPiece);
		myPlayerPieces[i] = newPiece->AddComponent<Transform>(Math::Vector3f(0.0f, i * -myBodyOffset, 0.0f), Math::Vector3f(-90.0f, 0.0f, 0.0f));
		myRotationHistory[i] = myPlayerPieces[i]->GetRotation().z;
		myPositionHistory[i] = myPlayerPieces[i]->GetTranslation();

		if (i == 0)
		{
			newPiece->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("SM_WormHead.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("Worm.MAT")->material);
			myPlayerPieces[i]->AddTranslation(0.0f, 0.0f, -100.0f);
			newPiece->AddComponent<BoxCollider>(true, Math::Vector3(150.0f, 200.0f, 200.0f), Math::Vector3f(0.0f, 0.0f, 50.0f));
		}
		else if (i == max_pieces - 1)
		{
			newPiece->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("SM_WormTail.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("Worm.MAT")->material);
			newPiece->AddComponent<BoxCollider>(true, Math::Vector3(150.0f, 200.0f, 200.0f), Math::Vector3f(0.0f, 0.0f, 50.0f));
		}
		else
		{
			newPiece->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("SM_WormBody.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("Worm.MAT")->material);
			newPiece->AddComponent<BoxCollider>(true, Math::Vector3(150.0f, 200.0f, 200.0f), Math::Vector3f(0.0f, 0.0f, 50.0f));
		}
	}
}

void Worm::ScrollBackgroundPlane()
{
	myBackgroundPlane1->AddTranslation(0.0f, -myEnvironmentScrollSpeed * Engine::Get().GetTimer().GetDeltaTime(), 0.0f);
	myBackgroundPlane2->AddTranslation(0.0f, -myEnvironmentScrollSpeed * Engine::Get().GetTimer().GetDeltaTime(), 0.0f);

	if (myBackgroundPlane1->GetTranslation().y < -myCameraViewportDimensions.y - 5000.0f)
		myBackgroundPlane1->SetTranslation(myBackgroundPlane1->GetTranslation().x, 10000.0f, myBackgroundPlane1->GetTranslation().z);

	if (myBackgroundPlane2->GetTranslation().y < -myCameraViewportDimensions.y - 5000.0f)
		myBackgroundPlane2->SetTranslation(myBackgroundPlane2->GetTranslation().x, 10000.0f, myBackgroundPlane2->GetTranslation().z);
}

void Worm::MovePlayerHead()
{
	float moveVectorLength = myEnvironmentScrollSpeed * 2.0f * Engine::Get().GetTimer().GetDeltaTime();
	Math::Vector3f moveVector = myPlayerPieces[0]->GetForwardVector() * moveVectorLength;
	moveVector.y -= myEnvironmentScrollSpeed * Engine::Get().GetTimer().GetDeltaTime();
	myPlayerPieces[0]->AddTranslation(moveVector);
	myDistanceTravelled += moveVectorLength;

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
	float dir = Math::Sign(Math::ToVector3(diff).GetNormalized().Dot(myPlayerPieces[0]->GetRightVector(true)));
	float newZRot = -dir * myMaxTurnRate;
	myPlayerPieces[0]->AddRotation({ 0.0f, 0.0f, newZRot });
}

void Worm::UpdatePlayerBodyPosition(const float aT)
{
	for (auto& pos : myPositionHistory)
	{
		pos.y -= myEnvironmentScrollSpeed * Engine::Get().GetTimer().GetDeltaTime();
	}

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

void Worm::CreateObstacles()
{
	auto& sceneHandler = Engine::Get().GetSceneHandler();

	std::array<const char*, 7> myObstaclePaths = {
		"SM_BranchLarge.fbx",
		"SM_BranchMedium.fbx",
		"SM_BranchSmall.fbx",
		"SM_StoneLarge.fbx",
		"SM_StoneMedium.fbx",
		"SM_StoneSmall.fbx",
		"SM_Button.fbx"
	};

	std::array<const char*, 7> myObstacleMatPaths = {
		"Branches.MAT",
		"Branches.MAT",
		"Branches.MAT",
		"Stones.MAT",
		"Stones.MAT",
		"Stones.MAT",
		"Button.MAT"
	};

	std::array<Math::Vector3f, 7> myColliderExtents = {
		Math::Vector3f(200.0f, 200.0f, 1700.0f),
		Math::Vector3f(150.0f, 200.0f, 1200.0f),
		Math::Vector3f(100.0f, 100.0f, 800.0f),
		Math::Vector3f(900.0f, 900.0f, 900.0f),
		Math::Vector3f(550.0f, 550.0f, 550.0f),
		Math::Vector3f(250.0f, 250.0f, 250.0f),
		Math::Vector3f(150.0f, 150.0f, 150.0f)
	};

	std::array<Math::Vector3f, 7> myColliderOffsets = {
		Math::Vector3f(0.0f, 0.0f, 350.0f),
		Math::Vector3f(0.0f, 0.0f, 170.0f),
		Math::Vector3f(0.0f, 0.0f, 70.0f),
		Math::Vector3f(0.0f, 0.0f, 0.0f),
		Math::Vector3f(0.0f, 0.0f, 0.0f),
		Math::Vector3f(0.0f, 0.0f, 0.0f),
		Math::Vector3f(0.0f, 0.0f, 0.0f)
	};

	for (int i = 0; i < max_obstacles; i++)
	{
		std::shared_ptr<GameObject> newObstacle = std::make_shared<GameObject>();
		sceneHandler.Instantiate(newObstacle);

		int obstacleType = Utilities::RandomInRange(0, static_cast<int>(myObstaclePaths.size()) - 1);
		auto meshAsset = AssetManager::Get().GetAsset<MeshAsset>(myObstaclePaths[obstacleType]);
		auto matAsset = AssetManager::Get().GetAsset<MaterialAsset>(myObstacleMatPaths[obstacleType]);
		newObstacle->AddComponent<Model>(meshAsset->mesh, matAsset->material);
		newObstacle->AddComponent<BoxCollider>(true, myColliderExtents[obstacleType], myColliderOffsets[obstacleType]);
		myObstacles[i] = newObstacle->AddComponent<Transform>(Math::Vector3f(0.0f, -5000.0f, 0.0f), Math::Vector3f(-90.0f, 0.0f, 0.0f));
		myObstacles[i]->SetUniformScale(Utilities::RandomInRange(0.5f, 2.0f));
		newObstacle->SetActive(false);
	}
}

void Worm::SpawnObstacle()
{
	if (myActiveObstacles >= max_obstacles - 1) return;

	auto obstacle = GetUnusedObstacle();
	obstacle->SetTranslation(GetRandomSpawnPoint());
	obstacle->AddRotation(0.0f, 0.0f, Utilities::RandomInRange(-180.0f, 180.0f));
	obstacle->gameObject->SetActive(true);
	++myActiveObstacles;
}

std::shared_ptr<Transform> Worm::GetUnusedObstacle()
{
	for (auto& obstacle : myObstacles)
	{
		if (!obstacle->gameObject->GetActive())
			return obstacle;
	}

	return std::shared_ptr<Transform>();
}

Math::Vector3f Worm::GetRandomSpawnPoint()
{
	Math::Vector3f spawnPoint;
	spawnPoint.x = Utilities::RandomInRange(-myCameraViewportDimensions.x, myCameraViewportDimensions.x);
	float min = myLastObstacleSpawnPoint.x - myMinObstacleXDiff;
	float max = myLastObstacleSpawnPoint.x + myMinObstacleXDiff;
	if (Math::IsInRange(spawnPoint.x, min, max))
	{
		if (spawnPoint.x - min < max - spawnPoint.x)
			spawnPoint.x = min;
		else
			spawnPoint.x = max;
	}

	spawnPoint.y = myCameraViewportDimensions.y + myObstacleSpawnYOffset;
	spawnPoint.z = 0.0f;

	myLastObstacleSpawnPoint = spawnPoint;
	return myLastObstacleSpawnPoint;
}

void Worm::MoveObstacles()
{
	float dt = Engine::Get().GetTimer().GetDeltaTime();

	for (auto& obstacle : myObstacles)
	{
		if (!obstacle->gameObject->GetActive()) continue;

		obstacle->AddTranslation(0.0f, -myEnvironmentScrollSpeed * dt, 0.0f);

		if (obstacle->GetTranslation().y < -myCameraViewportDimensions.y - myObstacleSpawnYOffset)
		{
			obstacle->gameObject->SetActive(false);
			--myActiveObstacles;
		}
	}
}