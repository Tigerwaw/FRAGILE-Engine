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

int currentDebugMode = 0;

Application* CreateApplication()
{
	AppSettings::LoadSettings(std::filesystem::current_path() / APP_SETTINGS_PATH);
    return new Worm();
}

void Worm::InitializeApplication()
{
	InputHandler& inputHandler = Engine::Get().GetInputHandler();
	inputHandler.SetCursorVisibility(false);

	inputHandler.RegisterAnalog2DAction("MousePosNDC", MouseMovement2D::MousePosNDC);
	inputHandler.RegisterBinaryAction("Space", Keys::SPACE, GenericInput::ActionType::Clicked);
#ifdef _DEBUG
	inputHandler.RegisterBinaryAction("F6", Keys::F6, GenericInput::ActionType::Clicked);
	inputHandler.RegisterBinaryAction("F5", Keys::F5, GenericInput::ActionType::Clicked);
	inputHandler.RegisterBinaryAction("F4", Keys::F4, GenericInput::ActionType::Clicked);
#endif

	auto& sceneHandler = Engine::Get().GetSceneHandler();
	sceneHandler.LoadScene("WormGame/WormGame.SCENE");

	myBackgroundPlane1 = sceneHandler.FindGameObjectByName("Plane1")->GetComponent<Transform>();
	myBackgroundPlane2 = sceneHandler.FindGameObjectByName("Plane2")->GetComponent<Transform>();

	myCameraViewportDimensions = Camera::GetMainCamera()->GetViewportDimensions();

	std::shared_ptr<GameObject> reticle = std::make_shared<GameObject>();
	sceneHandler.Instantiate(reticle);
	myReticle = reticle->AddComponent<Transform>();
	myReticle->SetUniformScale(50.0f);
	reticle->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("SM_sphere.fbx")->mesh);

	CreateWorm();
	CreateObstacles();
}

void Worm::UpdateApplication()
{
	if (!myIsGameRunning)
	{
		if (Engine::Get().GetInputHandler().GetBinaryAction("Space"))
			StartGame();
		else
			return;
	}

	UpdateReticle();
	IncreaseDifficulty();
	MoveEverything();
}

void Worm::UpdateDebug()
{
	if (Engine::Get().GetInputHandler().GetBinaryAction("F4"))
	{
		SpawnObstacle();
	}

	if (Engine::Get().GetInputHandler().GetBinaryAction("F5"))
	{
		Engine::Get().DrawColliders = !Engine::Get().DrawColliders;
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

	if (ImGui::Begin("Position history"))
	{
		ImGui::Text("FPS: %i", Engine::Get().GetTimer().GetAverageFPS());
		ImGui::Text("Frametime: %.2fms", Engine::Get().GetTimer().GetAverageFrameTimeMS());
		ImGui::Text("Drawcalls: %i", GraphicsEngine::Get().GetDrawcallAmount());

		ImGui::Text("Scroll Speed: %.2f", myEnvironmentScrollSpeed);
		ImGui::Text("Scroll Speed Increase: %.2f", myEnvironmentSpeedIncreaseMultiplier);
		ImGui::Text("Obstacle Spawn Timer: %.2f", myObstacleSpawnTimer);

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

void Worm::StartGame()
{
	myEnvironmentSpeedIncreaseMultiplier = myEnvironmentSpeedIncreaseStartMultiplier;
	myObstacleSpawnTimer = myObstacleStartSpawnTimer;
	myEnvironmentScrollSpeed = myEnvironmentStartingSpeed;
	myTimeSinceLastObstacleSpawned = 0.0f;
	myTimeSinceSpawnrateIncrease = 0.0f;

	for (auto& obstacle : myObstacles)
	{
		if (obstacle->gameObject->GetActive())
		{
			obstacle->gameObject->SetActive(false);
			--myActiveObstacles;
		}
	}

	for (int i = 0; i < static_cast<int>(myPlayerPieces.size()); i++)
	{
		myPlayerPieces[i]->SetTranslation(0.0f, i * -myBodyOffset, 0.0f);
		myPlayerPieces[i]->SetRotation(-90.0f, 0.0f, 0.0f);
		myRotationHistory[i] = myPlayerPieces[i]->GetRotation().z;
		myPositionHistory[i] = myPlayerPieces[i]->GetTranslation();
	}

	myIsGameRunning = true;
}

void Worm::GameOver()
{
	printf("Game Over\n");
	myIsGameRunning = false;
}

void Worm::UpdateReticle()
{
	InputHandler& input = Engine::Get().GetInputHandler();
	Math::Vector2f mousePosNDC = input.GetAnalogAction2D("MousePosNDC");
	Math::Vector2f mouseInWorldPos = mousePosNDC * myCameraViewportDimensions + Math::ToVector2(Camera::GetMainCamera()->gameObject->GetComponent<Transform>()->GetTranslation());
	myReticle->SetTranslation(mouseInWorldPos.x, mouseInWorldPos.y, -500.0f);
}

void Worm::MoveEverything()
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

void Worm::IncreaseDifficulty()
{
	float dt = Engine::Get().GetTimer().GetDeltaTime();

	myEnvironmentSpeedIncreaseMultiplier += dt * myEnvironmentSpeedIncreaseMultiplierIncrease;
	myEnvironmentScrollSpeed += dt * myEnvironmentSpeedIncreaseMultiplier;

	myTimeSinceSpawnrateIncrease += dt;
	if (myTimeSinceSpawnrateIncrease > mySpawnrateIncreaseTimer)
	{
		myObstacleSpawnTimer = std::clamp(myObstacleSpawnTimer - myObstacleSpawnTimerReduction, myObstacleMinSpawnTimer, myObstacleStartSpawnTimer);
		myTimeSinceSpawnrateIncrease = 0.0f;
	}
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

		std::shared_ptr<BoxCollider> collider;

		if (i == 0)
		{
			newPiece->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("SM_WormHead.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("Worm.MAT")->material);
			myPlayerPieces[i]->AddTranslation(0.0f, 0.0f, -100.0f);
			collider = newPiece->AddComponent<BoxCollider>(true, Math::Vector3(120.0f, 100.0f, 120.0f), Math::Vector3f(0.0f, 0.0f, 0.0f));
		}
		else if (i == max_pieces - 1)
		{
			newPiece->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("SM_WormTail.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("Worm.MAT")->material);
			collider = newPiece->AddComponent<BoxCollider>(true, Math::Vector3(120.0f, 100.0f, 120.0f), Math::Vector3f(0.0f, 0.0f, 50.0f));
		}
		else
		{
			newPiece->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("SM_WormBody.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("Worm.MAT")->material);
			collider = newPiece->AddComponent<BoxCollider>(true, Math::Vector3(120.0f, 100.0f, 150.0f), Math::Vector3f(0.0f, 0.0f, 100.0f));
		}

		collider->SetOnTriggerEnterResponse([this, collider](Collider::CollisionInfo aCollisionInfo)
			{
				Collider* otherCollider = aCollisionInfo.collA;

				if (otherCollider == collider.get())
					otherCollider = aCollisionInfo.collB;

				if (otherCollider->gameObject->GetName() == "Obstacle")
				{
					GameOver();
				}
			});
	}
}

void Worm::ScrollBackgroundPlane()
{
	myBackgroundPlane1->AddTranslation(0.0f, -myEnvironmentScrollSpeed * Engine::Get().GetTimer().GetDeltaTime(), 0.0f);
	myBackgroundPlane2->AddTranslation(0.0f, -myEnvironmentScrollSpeed * Engine::Get().GetTimer().GetDeltaTime(), 0.0f);

	if (myBackgroundPlane1->GetTranslation().y < -myCameraViewportDimensions.y - 5000.0f)
		myBackgroundPlane1->SetTranslation(myBackgroundPlane1->GetTranslation().x, 9000.0f, myBackgroundPlane1->GetTranslation().z);

	if (myBackgroundPlane2->GetTranslation().y < -myCameraViewportDimensions.y - 5000.0f)
		myBackgroundPlane2->SetTranslation(myBackgroundPlane2->GetTranslation().x, 9000.0f, myBackgroundPlane2->GetTranslation().z);
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

	if (myPlayerPieces[0]->GetTranslation().y < -myCameraViewportDimensions.y)
	{
		GameOver();
	}
}

void Worm::UpdatePlayerHeadRotation()
{
	InputHandler& input = Engine::Get().GetInputHandler();
	Math::Vector2f mousePosNDC = input.GetAnalogAction2D("MousePosNDC");
	Math::Vector2f adjustedPos = mousePosNDC * myCameraViewportDimensions + Math::ToVector2(Camera::GetMainCamera()->gameObject->GetComponent<Transform>()->GetTranslation());

	Math::Vector2f diff = adjustedPos - Math::ToVector2(myPlayerPieces[0]->GetTranslation());
	float dot = Math::ToVector3(diff).GetNormalized().Dot(myPlayerPieces[0]->GetRightVector(true));
	float turnRate = myEnvironmentScrollSpeed * 2.0f * myMaxTurnRate;
	float newZRot = -dot * turnRate * Engine::Get().GetTimer().GetDeltaTime();
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

	std::array<const char*, 6> myObstaclePaths = {
		"SM_BranchLarge.fbx",
		"SM_BranchMedium.fbx",
		"SM_BranchSmall.fbx",
		"SM_LargeStone.fbx",
		"SM_MediumStone.fbx",
		"SM_SmallStone.fbx"
	};

	std::array<const char*, 6> myObstacleMatPaths = {
		"Branches.MAT",
		"Branches.MAT",
		"Branches.MAT",
		"Stones.MAT",
		"Stones.MAT",
		"Stones.MAT",
	};

	std::array<Math::Vector3f, 6> myColliderExtents = {
		Math::Vector3f(120.0f, 200.0f, 1500.0f),
		Math::Vector3f(100.0f, 200.0f, 1000.0f),
		Math::Vector3f(60.0f, 100.0f, 700.0f),
		Math::Vector3f(800.0f, 900.0f, 900.0f),
		Math::Vector3f(200.0f, 550.0f, 600.0f),
		Math::Vector3f(250.0f, 250.0f, 150.0f)
	};

	std::array<Math::Vector3f, 6> myColliderOffsets = {
		Math::Vector3f(-20.0f, 0.0f, 350.0f),
		Math::Vector3f(0.0f, 0.0f, 170.0f),
		Math::Vector3f(0.0f, 0.0f, 70.0f),
		Math::Vector3f(0.0f, 0.0f, 0.0f),
		Math::Vector3f(0.0f, 0.0f, 0.0f),
		Math::Vector3f(0.0f, 0.0f, 0.0f)
	};

	for (int i = 0; i < max_obstacles; i++)
	{
		std::shared_ptr<GameObject> newObstacle = std::make_shared<GameObject>();
		newObstacle->SetName("Obstacle");
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
	float playerMovingRight = Math::Sign(myPlayerPieces[0]->GetForwardVector(true).Dot({ 1.0f, 0.0f, 0.0f }));
	Math::Vector3f initialPos = myPlayerPieces[0]->GetTranslation();
	Math::Vector3f predictedPos = myPlayerPieces[0]->GetTranslation() + Math::Vector3f(1.0f, 0.0f, 0.0f) * myMinObstacleXDiff * playerMovingRight;

	Math::Vector3f spawnPoint;
	spawnPoint.x = Utilities::RandomInRange(std::min(initialPos.x, predictedPos.x), std::max(initialPos.x, predictedPos.x));
	spawnPoint.y = myCameraViewportDimensions.y + myObstacleSpawnYOffset;
	spawnPoint.z = 0.0f;
	return spawnPoint;
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