#pragma once
#include "GameEngine/Application/Application.h"
#include "GameEngine/Application/EntryPoint.h"

#include <memory>
#include <array>

class GameObject;
class Transform;

constexpr int max_pieces = 10;
constexpr int max_obstacles = 30;

class Worm : public Application
{
public:
    void InitializeApplication() override;
    void UpdateApplication() override;
    void UpdateDebug() override;

private:
    void CreateWorm();
    void ScrollBackgroundPlane();
    void MovePlayerHead();
    void UpdatePlayerHeadRotation();
    void UpdatePlayerBodyPosition(const float aT);
    void UpdateRotationHistory(const float aNewRotation);
    void UpdatePositionHistory(const Math::Vector3f& aNewPosition);

    std::shared_ptr<Transform> myBackgroundPlane1;
    std::shared_ptr<Transform> myBackgroundPlane2;
    std::array<std::shared_ptr<Transform>, max_pieces> myPlayerPieces;
    std::array<float, max_pieces> myRotationHistory;
    std::array<Math::Vector3f, max_pieces> myPositionHistory;

    float myBodyOffset = 180.0f;
    
    float myEnvironmentScrollSpeed = 300.0f;

    float myMaxTurnRate = 3.0f;

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
    float myObstacleSpawnTimer = 1.0f;
    float myTimeSinceLastObstacleSpawned = 0.0f;
    int myActiveObstacles = 0;
    Math::Vector3f myLastObstacleSpawnPoint;
    float myMinObstacleXDiff = 600.0f;
};