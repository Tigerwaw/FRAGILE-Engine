#include "Enginepch.h"

#include "Transform.h"
#include "CommonUtilities/SerializationUtils.hpp"

Transform::Transform(Math::Vector3f aPosition, Math::Vector3f aRotation, Math::Vector3f aScale)
{
	myPosition = aPosition;
	myRotation = aRotation;
	myScale = aScale;
}

void Transform::Start()
{

}

void Transform::Update()
{

}

void Transform::UpdateToWorldMatrix(const Math::Matrix4x4f& aToWorldMatrix, const Math::Matrix4x4f& aToWorldMatrixNoScale)
{
	myToWorldMatrix = aToWorldMatrix;
	myToWorldMatrixNoScale = aToWorldMatrixNoScale;

	UpdateChildrenToWorldMatrix();
}

void Transform::UpdateChildrenToWorldMatrix()
{
	for (auto& child : myChildren)
	{
		child->UpdateToWorldMatrix(GetMatrixInternal(false) * myToWorldMatrix, GetMatrixInternal(true) * myToWorldMatrixNoScale);
	}
}

void Transform::SetIsDirty()
{
	myIsDirty = true;

	UpdateChildrenToWorldMatrix();
}

const Math::Matrix4x4f& Transform::GetMatrix()
{
	if (IsScaled())
	{
		return GetMatrixInternal(false);
	}
	else
	{
		return GetMatrixInternal(true);
	}
}

const Math::Matrix4x4f Transform::GetWorldMatrix()
{
	if (IsScaled())
	{
		return GetWorldMatrixInternal(false);
	}
	else
	{
		return GetWorldMatrixInternal(true);
	}
}

const Math::Matrix4x4f Transform::GetMatrixInverse()
{
	if (IsScaled())
	{
		return GetMatrixInternal(false).GetInverse();
	}
	else
	{
		return GetMatrixInternal(true).GetFastInverse();
	}
}

const Math::Matrix4x4f Transform::GetWorldMatrixInverse()
{
	if (IsScaled())
	{
		return GetWorldMatrixInternal(false).GetInverse();
	}
	else
	{
		return GetWorldMatrixInternal(true).GetFastInverse();
	}
}

void Transform::SetParent(Transform* aTransform)
{
	if (myParent)
	{
		myParent->RemoveChild(this);

		if (aTransform)
		{
			aTransform->AddChild(this);
		}

		myParent = aTransform;
	}
	else
	{
		if (aTransform)
		{
			aTransform->AddChild(this);
		}

		myParent = aTransform;
	}

	if (myParent)
	{
		myParent->UpdateChildrenToWorldMatrix();
	}
	else
	{
		myToWorldMatrix = Math::Matrix4x4f();
		myToWorldMatrixNoScale = Math::Matrix4x4f();
		UpdateChildrenToWorldMatrix();
	}
}

void Transform::SetParentInternal(Transform* aTransform)
{
	if (myParent)
	{
		myParent->RemoveChild(this);
		myParent = aTransform;
	}
	else
	{
		myParent = aTransform;
	}

	if (myParent)
	{
		myParent->UpdateChildrenToWorldMatrix();
	}
	else
	{
		myToWorldMatrix = Math::Matrix4x4f();
		myToWorldMatrixNoScale = Math::Matrix4x4f();
		UpdateChildrenToWorldMatrix();
	}
}

void Transform::AddChild(Transform* aTransform)
{
	if (!aTransform) return;

	for (int i = 0; i < myChildren.size(); i++)
	{
		if (myChildren.empty()) break;
		if (myChildren[i] == aTransform) return;
	}

	aTransform->SetParentInternal(this);
	myChildren.emplace_back(aTransform);
	UpdateChildrenToWorldMatrix();
}

void Transform::RemoveChild(Transform* aTransform)
{
	if (!aTransform) return;
	if (myChildren.empty()) return;

	for (int i = 0; i < myChildren.size(); i++)
	{
		if (myChildren[i] == aTransform)
		{
			aTransform->myParent = nullptr;
			myChildren.erase(myChildren.begin() + i);
			break;
		}
	}
}

const Math::Vector3f Transform::GetRightVector(bool aInWorldSpace)
{
	if (aInWorldSpace)
	{
		return GetWorldMatrix().GetRightVector();
	}
	else
	{
		return GetMatrix().GetRightVector();
	}
}

const Math::Vector3f Transform::GetUpVector(bool aInWorldSpace)
{
	if (aInWorldSpace)
	{
		return GetWorldMatrix().GetUpVector();
	}
	else
	{
		return GetMatrix().GetUpVector();
	}
}

const Math::Vector3f Transform::GetForwardVector(bool aInWorldSpace)
{
	if (aInWorldSpace)
	{
		return GetWorldMatrix().GetForwardVector();
	}
	else
	{
		return GetMatrix().GetForwardVector();
	}
}

const Math::Vector3f Transform::GetTranslation(bool aInWorldSpace) const
{
	if (aInWorldSpace)
	{
		return Math::ToVector3(Math::ToVector4(myPosition) * myToWorldMatrix);
	}
	else
	{
		return myPosition;
	}
}

const Math::Vector3f Transform::GetRotation(bool aInWorldSpace) const
{
	if (aInWorldSpace)
	{
		return Math::ToVector3(Math::ToVector4(myRotation) * myToWorldMatrix);
	}
	else
	{
		return myRotation;
	}
}

const Math::Vector3f Transform::GetScale(bool aInWorldSpace) const
{
	if (aInWorldSpace)
	{
		return Math::ToVector3(Math::ToVector4(myScale) * myToWorldMatrix);
	}
	else
	{
		return myScale;
	}
}

void Transform::SetTranslation(const Math::Vector3f aTranslation)
{
	myPosition = aTranslation;
	SetIsDirty();
}

void Transform::SetTranslation(const float aX, const float aY, const float aZ)
{
	myPosition = { aX, aY, aZ };
	SetIsDirty();
}

void Transform::AddTranslation(const Math::Vector3f aTranslation)
{
	myPosition += aTranslation;
	SetIsDirty();
}

void Transform::AddTranslation(const float aX, const float aY, const float aZ)
{
	myPosition += { aX, aY, aZ };
	SetIsDirty();
}

void Transform::SetRotation(const Math::Vector3f aRotationInDegrees)
{
	myRotation = aRotationInDegrees;
	SetIsDirty();
}

void Transform::SetRotation(const float aPitch, const float aYaw, const float aRoll)
{
	myRotation.x = aPitch;
	myRotation.y = aYaw;
	myRotation.z = aRoll;
	SetIsDirty();
}

void Transform::AddRotation(const Math::Vector3f aRotationInDegrees)
{
	myRotation += aRotationInDegrees;
	SetIsDirty();
}

void Transform::AddRotation(const float aPitch, const float aYaw, const float aRoll)
{
	myRotation.x += aPitch;
	myRotation.y += aYaw;
	myRotation.z += aRoll;
	SetIsDirty();
}

void Transform::SetScale(const Math::Vector3f aScale)
{
	myScale = aScale;
	SetIsDirty();
}

void Transform::SetScale(const float aX, const float aY, const float aZ)
{
	myScale = { aX, aY, aZ };
	SetIsDirty();
}

void Transform::SetUniformScale(float aScale)
{
	myScale = { aScale, aScale, aScale };
	SetIsDirty();
}

void Transform::AddScale(const Math::Vector3f aScale)
{
	myScale += aScale;
	SetIsDirty();
}

void Transform::AddScale(const float aX, const float aY, const float aZ)
{
	myScale += { aX, aY, aZ };
	SetIsDirty();
}

void Transform::AddUniformScale(float aScale)
{
	myScale += { aScale, aScale, aScale };
	SetIsDirty();
}

const bool Transform::IsScaled() const
{
	float tolerance = 0.001f;
	float scaleLength = myScale.LengthSqr();

	return (scaleLength < (3.0f - tolerance) || scaleLength > (3.0f + tolerance));
}

bool Transform::Serialize(nl::json& outJsonObject)
{
	outJsonObject;
	return false;
}

bool Transform::Deserialize(nl::json& aJsonObject)
{
	Math::Vector3f pos;
	Math::Vector3f rot;
	Math::Vector3f scale = { 1.0f, 1.0f, 1.0f };

	if (aJsonObject.contains("Position"))
	{
		SetTranslation(Utilities::DeserializeVector3<float>(aJsonObject["Position"]));
	};

	if (aJsonObject.contains("Rotation"))
	{
		SetRotation(Utilities::DeserializeVector3<float>(aJsonObject["Rotation"]));
	}

	if (aJsonObject.contains("Scale"))
	{
		SetScale(Utilities::DeserializeVector3<float>(aJsonObject["Scale"]));
	}

	return true;
}

const Math::Matrix4x4f& Transform::GetMatrixInternal(bool aNoScale)
{
	if (myIsDirty)
	{
		myCachedMatrix = Math::Matrix4x4f();
		myCachedMatrixNoScale = Math::Matrix4x4f();

		Math::Matrix4x4f scaleMatrix;
		scaleMatrix(1, 1) = myScale.x;
		scaleMatrix(2, 2) = myScale.y;
		scaleMatrix(3, 3) = myScale.z;

		myCachedMatrix *= scaleMatrix;

		Math::Matrix4x4f rotationMatrix = Math::Matrix4x4f::CreateRollPitchYawMatrix(myRotation);
		myCachedMatrix *= rotationMatrix;
		myCachedMatrixNoScale *= rotationMatrix;

		Math::Matrix4x4f translationMatrix;
		translationMatrix(4, 1) = myPosition.x;
		translationMatrix(4, 2) = myPosition.y;
		translationMatrix(4, 3) = myPosition.z;

		myCachedMatrix *= translationMatrix;
		myCachedMatrixNoScale *= translationMatrix;

		myIsDirty = false;
	}

	if (aNoScale)
	{
		return myCachedMatrixNoScale;
	}
	else
	{
		return myCachedMatrix;
	}
}

const Math::Matrix4x4f Transform::GetWorldMatrixInternal(bool aNoScale)
{
	if (aNoScale)
	{
		return GetMatrixInternal(true) * myToWorldMatrixNoScale;
	}
	else
	{
		return GetMatrixInternal(false) * myToWorldMatrix;
	}
}
