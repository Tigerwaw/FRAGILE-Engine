#pragma once
#include "ComponentSystem/Component.h"
#include "Math/Vector.hpp"

class Mesh;
class Texture;

class AmbientLight : public Component
{
public:
	AmbientLight() = default;
	AmbientLight(std::shared_ptr<Texture> aCubemap, Math::Vector3f aColor = {1.0f, 1.0f, 1.0f}, float aIntensity = 1.0f);
	void Start() override;
	void Update() override;
	void SetColor(Math::Vector3f aColor);
	void SetIntensity(float aIntensity);
	void SetCubemap(std::shared_ptr<Texture> aCubemap);
	std::shared_ptr<Texture> GetCubemap() const;
	std::shared_ptr<Mesh> GetSkyboxMesh() const;
	Math::Vector3f GetColor() const { return myColor; }
	float GetIntensity() const { return myIntensity; }

	bool Serialize(nl::json& outJsonObject) override;
	bool Deserialize(nl::json& aJsonObject) override;
private:
	std::shared_ptr<Texture> myCubemap;
	std::shared_ptr<Mesh> mySkyboxMesh;
	Math::Vector3f myColor = { 1.0f, 1.0f, 1.0f };
	float myIntensity = 1.0f;
};

