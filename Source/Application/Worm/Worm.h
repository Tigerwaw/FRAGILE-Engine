#pragma once
#include "GameEngine/Application/Application.h"
#include "GameEngine/Application/EntryPoint.h"

#include <memory>
#include <array>

class GameObject;
class Transform;

constexpr int max_pieces = 10;

class Worm : public Application
{
public:
    void InitializeApplication() override;
    void UpdateApplication() override;
    void UpdateDebug() override;

private:
    void ScrollBackgroundPlane();
    void MovePlayerHead();
    void UpdatePlayerHeadRotation();
    void UpdatePlayerBodyPosition(const float aT);
    void UpdateRotationHistory(const float aNewRotation);
    void UpdatePositionHistory(const Math::Vector3f& aNewPosition);

    std::shared_ptr<Transform> myBackgroundPlane;
    std::shared_ptr<Material> myBackgroundPlaneMaterial;
    std::array<std::shared_ptr<Transform>, max_pieces> myPlayerPieces;
    std::array<float, max_pieces> myRotationHistory;
    std::array<Math::Vector3f, max_pieces> myPositionHistory;

    float myBodyOffset = 180.0f;

    float myWormSpeed = 500.0f;
    float myBackgroundScrollSpeed = 0.0001f;

    float myMaxTurnRate = 1.0f;

    float myHighestYPoint = 0.0f;
    float myDistanceTravelled = 0.0f;

    int historyUpdated = 0;

    Math::Vector2f myCameraViewportDimensions;
};