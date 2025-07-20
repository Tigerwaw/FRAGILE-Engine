#pragma once
#include "ComponentSystem/Component.h"
#include "Math/Matrix4x4.hpp"
#include "Math/Vector.hpp"
#include "Math/Quaternion.hpp"



class Transform : public Component
{
public:
	Transform(Math::Vector3f aPosition = { 0, 0, 0 }, Math::Vector3f aRotation = { 0, 0, 0 }, Math::Vector3f aScale = { 1, 1, 1 });

    void Start() override;
    void Update() override;

	void SetParent(Transform* aTransform);
	void AddChild(Transform* aTransform);
	void RemoveChild(Transform* aTransform);

	const Transform* GetParent() const { return myParent; }
	const std::vector<Transform*> GetChildren() const { return myChildren; }

	const Math::Matrix4x4f& GetMatrix();
	const Math::Matrix4x4f GetWorldMatrix();
	const Math::Matrix4x4f GetMatrixInverse();
	const Math::Matrix4x4f GetWorldMatrixInverse();

	const Math::Vector3f GetRightVector(bool aInWorldSpace = false);
	const Math::Vector3f GetUpVector(bool aInWorldSpace = false);
	const Math::Vector3f GetForwardVector(bool aInWorldSpace = false);
	const Math::Vector3f GetTranslation(bool aInWorldSpace = false) const;
	const Math::Vector3f GetRotation(bool aInWorldSpace = false) const;
	const Math::Vector3f GetScale(bool aInWorldSpace = false) const;

	void SetTranslation(const Math::Vector3f aTranslation);
	void SetTranslation(const float aX, const float aY, const float aZ);
	void AddTranslation(const Math::Vector3f aTranslation);
	void AddTranslation(const float aX, const float aY, const float aZ);

	void SetRotation(const Math::Vector3f aRotationInDegrees);
	void SetRotation(const float aPitch, const float aYaw, const float aRoll);
	void AddRotation(const Math::Vector3f aRotationInDegrees);
	void AddRotation(const float aPitch, const float aYaw, const float aRoll);

	void SetScale(const Math::Vector3f aScale);
	void SetScale(const float aX, const float aY, const float aZ);
	void SetUniformScale(float aScale);
	void AddScale(const Math::Vector3f aScale);
	void AddScale(const float aX, const float aY, const float aZ);
	void AddUniformScale(float aScale);

	const bool IsScaled() const;

	bool Serialize(nl::json& outJsonObject) override;
	bool Deserialize(nl::json& aJsonObject) override;
private:
	const Math::Matrix4x4f& GetMatrixInternal(bool aNoScale);
	const Math::Matrix4x4f GetWorldMatrixInternal(bool aNoScale);
	void UpdateToWorldMatrix(const Math::Matrix4x4f& aToWorldMatrix, const Math::Matrix4x4f& aToWorldMatrixNoScale);
	void UpdateChildrenToWorldMatrix();
	void SetIsDirty();
	void SetParentInternal(Transform* aTransform);
	Math::Matrix4x4f myCachedMatrix;
	Math::Matrix4x4f myCachedMatrixNoScale;
	bool myIsDirty = true;

	Math::Matrix4x4f myToWorldMatrix;
	Math::Matrix4x4f myToWorldMatrixNoScale;

	Math::Vector3f myPosition;
	Math::Vector3f myRotation;
	Math::Vector3f myScale;

	Transform* myParent = nullptr;
	std::vector<Transform*> myChildren;
};