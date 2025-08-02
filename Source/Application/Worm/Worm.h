#pragma once
#include "GameEngine/Application/Application.h"
#include "GameEngine/Application/EntryPoint.h"

#include <memory>
#include <array>

class GameObject;
class Transform;
class TextComponent;

constexpr int max_pieces = 10;
constexpr int max_obstacles = 30;

class Worm : public Application
{
public:
    void InitializeApplication() override;
    void UpdateApplication() override;
    void UpdateDebug() override;

private:
    void CreateUI();
    void StartGame();
    void GameOver();
    void UpdateReticle();
    void MoveEverything();
    void IncreaseDifficulty();
    void CreateWorm();
    void ScrollBackgroundPlane();
    void MovePlayerHead();
    void UpdatePlayerHeadRotation();
    void UpdatePlayerBodyPosition(const float aT);
    void UpdateRotationHistory(const float aNewRotation);
    void UpdatePositionHistory(const Math::Vector3f& aNewPosition);

    bool myIsGameRunning = false;

    std::shared_ptr<Transform> myReticle;

    std::shared_ptr<Transform> myBackgroundPlane1;
    std::shared_ptr<Transform> myBackgroundPlane2;
    std::array<std::shared_ptr<Transform>, max_pieces> myPlayerPieces;
    std::array<float, max_pieces> myRotationHistory;
    std::array<Math::Vector3f, max_pieces> myPositionHistory;

    float myBodyOffset = 180.0f;
    
    float myEnvironmentStartingSpeed = 300.0f;
    float myEnvironmentScrollSpeed = 300.0f;
    float myEnvironmentSpeedIncreaseStartMultiplier = 1.0f;
    float myEnvironmentSpeedIncreaseMultiplier = 1.0f;
    float myEnvironmentSpeedIncreaseMultiplierIncrease = 0.1f;

    float mySpawnrateIncreaseTimer = 10.0f;
    float myTimeSinceSpawnrateIncrease = 0.0f;

    float myMaxTurnRate = 0.25f;

    float myScore = 0.0f;
    float myHighestYPoint = 0.0f;
    float myDistanceTravelled = 0.0f;

    int historyUpdated = 0;

    Math::Vector2f myCameraViewportDimensions;

    void CreateObstacles();
    void SpawnObstacle();
    std::shared_ptr<Transform> GetUnusedObstacle();
    Math::Vector3f GetRandomSpawnPoint();
    void MoveObstacles();
    std::array<std::shared_ptr<Transform>, max_obstacles> myObstacles;
    float myObstacleSpawnYOffset = 2000.0f;
    float myObstacleStartSpawnTimer = 3.0f;
    float myObstacleMinSpawnTimer = 1.0f;
    float myObstacleSpawnTimerReduction = 0.2f;
    float myObstacleSpawnTimer = 3.0f;
    float myTimeSinceLastObstacleSpawned = 0.0f;
    int myActiveObstacles = 0;
    float myMinObstacleXDiff = 600.0f;

    std::shared_ptr<TextComponent> myTitleText;
    std::shared_ptr<TextComponent> myTutorialText;
    std::shared_ptr<TextComponent> myPressSpaceText;
    std::shared_ptr<TextComponent> myScoreText;
};