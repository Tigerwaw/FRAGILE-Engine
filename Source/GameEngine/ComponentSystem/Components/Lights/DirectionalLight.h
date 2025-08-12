#pragma once
#include "ComponentSystem/Components/Lights/LightSource.h"
#include "Math/Vector.hpp"
#include "Math/AABB3D.hpp"


class GameObject;
class Camera;

class DirectionalLight : public LightSource
{
public:
	DirectionalLight(float aIntensity = 1.0f, Math::Vector3f aColor = { 1.0f, 1.0f, 1.0f });
	void RecalculateShadowFrustum(Camera* aRenderCamera, Math::AABB3D<float> aSceneBB);

	bool Serialize(nl::json& outJsonObject) override;
	bool Deserialize(nl::json& aJsonObject) override;
};

