#include "Enginepch.h"

#include "Camera.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"
#include "Math/Intersection3D.hpp"
#include "SceneHandler/SceneHandler.h"
#include <Math/MathConstants.hpp>
#include "Engine.h"
#include "Audio/AudioEngine.h"

static Camera* sInstance = nullptr;

Camera* Camera::GetMainCamera()
{
	return sInstance;
}

void Camera::SetAsMainCamera()
{
	sInstance = this;
	Engine::Get().GetAudioEngine().SetListener(Engine::Get().GetSceneHandler().FindGameObjectByID(gameObject->GetID()));
}

bool Camera::IsMainCamera() const
{
	return sInstance == this;
}

Camera::Camera(float aFOV, float aNearPlane, float aFarPlane, Math::Vector2f aResolution)
{
	InitPerspectiveProjection(aFOV, aNearPlane, aFarPlane, aResolution);
}

Camera::Camera(float aLeft, float aRight, float aTop, float aBottom, float aNear, float aFar)
{
	InitOrtographicProjection(aLeft, aRight, aTop, aBottom, aNear, aFar);
}

void Camera::Start()
{
	if (sInstance == nullptr)
	{
		SetAsMainCamera();
	}
}

void Camera::Update()
{
}

void Camera::InitPerspectiveProjection(float aFOV, float aNearPlane, float aFarPlane, Math::Vector2f aResolution)
{
	myNearPlane = aNearPlane;
	myFarPlane = aFarPlane;
	float fov = aFOV * Math::DEGREES_TO_RADIANS;
	float aspectRatio = aResolution.x / aResolution.y;
	float horizontalFOV = 1 / tanf(fov / 2);
	myHFOV = horizontalFOV;
	float verticalFOV = horizontalFOV * aspectRatio;
	myViewportDimensions = aResolution;

	myProjectionMatrix = Math::Matrix4x4<float>();
	myProjectionMatrix(1, 1) = horizontalFOV;
	myProjectionMatrix(2, 2) = verticalFOV;
	myProjectionMatrix(3, 3) = aFarPlane / (aFarPlane - aNearPlane);
	myProjectionMatrix(3, 4) = 1.0f;
	myProjectionMatrix(4, 3) = -aNearPlane * myProjectionMatrix(3, 3);
	myProjectionMatrix(4, 4) = 0;

	float nearHalfWidth = tan(fov / 2) * aNearPlane;
	float nearHalfHeight = nearHalfWidth * aspectRatio;

	float farHalfWidth = tan(fov / 2) * aFarPlane;
	float farHalfHeight = farHalfWidth * aspectRatio;

	myFrustumCorners[0] = { -nearHalfWidth, -nearHalfHeight, aNearPlane };
	myFrustumCorners[1] = { -nearHalfWidth, nearHalfHeight, aNearPlane };
	myFrustumCorners[2] = { nearHalfWidth, nearHalfHeight, aNearPlane };
	myFrustumCorners[3] = { nearHalfWidth, -nearHalfHeight, aNearPlane };
	myFrustumCorners[4] = { -farHalfWidth, -farHalfHeight, aFarPlane };
	myFrustumCorners[5] = { -farHalfWidth, farHalfHeight, aFarPlane };
	myFrustumCorners[6] = { farHalfWidth, farHalfHeight, aFarPlane };
	myFrustumCorners[7] = { farHalfWidth, -farHalfHeight, aFarPlane };
}

void Camera::InitOrtographicProjection(float aLeft, float aRight, float aTop, float aBottom, float aNearPlane, float aFarPlane)
{
	myViewportDimensions = { (aRight - aLeft) * 0.5f, (aTop - aBottom) * 0.5f };
	myNearPlane = aNearPlane;
	myFarPlane = aFarPlane;
	myProjectionMatrix = Math::Matrix4x4<float>();
	myProjectionMatrix(1, 1) = 2.0f / (aRight - aLeft);
	myProjectionMatrix(2, 2) = 2.0f / (aTop - aBottom);
	myProjectionMatrix(3, 3) = 1.0f / (aFarPlane - aNearPlane);

	myProjectionMatrix(4, 1) = -(aRight + aLeft) / (aRight - aLeft);
	myProjectionMatrix(4, 2) = -(aTop + aBottom) / (aTop - aBottom);
	myProjectionMatrix(4, 3) = -(aNearPlane) / (aFarPlane - aNearPlane);

	myFrustumCorners[0] = Math::Vector3f(aLeft, aBottom, aNearPlane);
	myFrustumCorners[1] = Math::Vector3f(aLeft, aTop, aNearPlane);
	myFrustumCorners[2] = Math::Vector3f(aRight, aTop, aNearPlane);
	myFrustumCorners[3] = Math::Vector3f(aRight, aBottom, aNearPlane);
	myFrustumCorners[4] = Math::Vector3f(aLeft, aBottom, aFarPlane);
	myFrustumCorners[5] = Math::Vector3f(aLeft, aTop, aFarPlane);
	myFrustumCorners[6] = Math::Vector3f(aRight, aTop, aFarPlane);
	myFrustumCorners[7] = Math::Vector3f(aRight, aBottom, aFarPlane);
}


Math::PlaneVolume<float> Camera::GetFrustumPlaneVolume(Math::Matrix4x4f aToObjectSpace)
{
	PIXScopedEvent(PIX_COLOR_INDEX(6), "Get Frustum Plane Volume");
	Math::Matrix4x4f matrix = gameObject->GetComponent<Transform>()->GetWorldMatrix() * aToObjectSpace;
	
	std::array<Math::Vector3f, 8> corners;
	for (int i = 0; i < myFrustumCorners.size(); i++)
	{
		corners[i] = Math::ToVector3<float>(Math::ToVector4<float>(myFrustumCorners[i], 1.0f) * matrix);
	}

	Math::PlaneVolume<float> volume;
	volume.AddPlane(Math::Plane<float>(corners[0], corners[1], corners[2]));
	volume.AddPlane(Math::Plane<float>(corners[6], corners[5], corners[4]));
	
	volume.AddPlane(Math::Plane<float>(corners[4], corners[5], corners[1]));
	volume.AddPlane(Math::Plane<float>(corners[2], corners[6], corners[7]));
	
	volume.AddPlane(Math::Plane<float>(corners[7], corners[4], corners[0]));
	volume.AddPlane(Math::Plane<float>(corners[1], corners[5], corners[6]));

	return volume;
}

bool Camera::GetViewcullingIntersection(std::shared_ptr<Transform> aObjectTransform, const Math::AABB3D<float>& aObjectAABB)
{
	return Math::IntersectionBetweenPlaneVolumeAABB(GetFrustumPlaneVolume(aObjectTransform->GetWorldMatrixInverse()), aObjectAABB);
}

bool Camera::Serialize(nl::json& outJsonObject)
{
	outJsonObject;
	return false;
}

bool Camera::Deserialize(nl::json& aJsonObject)
{
	if (aJsonObject.contains("Perspective"))
	{
		bool isPerpective = aJsonObject["Perspective"].get<bool>();
		if (isPerpective)
		{
			float fov = 0;
			float nearPlane = 0;
			float farPlane = 0;
			Math::Vector2f resolution = Engine::Get().GetResolution();

			if (aJsonObject.contains("FOV"))
			{
				fov = aJsonObject["FOV"].get<float>();
			}

			if (aJsonObject.contains("NearPlane"))
			{
				nearPlane = aJsonObject["NearPlane"].get<float>();
			}

			if (aJsonObject.contains("FarPlane"))
			{
				farPlane = aJsonObject["FarPlane"].get<float>();
			}

			if (aJsonObject.contains("Resolution"))
			{
				resolution = { aJsonObject["Resolution"][0].get<float>(), aJsonObject["Resolution"][1].get<float>() };
			}

			InitPerspectiveProjection(fov, nearPlane, farPlane, resolution);
		}
		else
		{
			float leftPlane = 0;
			float rightPlane = 0;
			float topPlane = 0;
			float bottomPlane = 0;
			float nearPlane = 0;
			float farPlane = 0;

			if (aJsonObject.contains("LeftPlane"))
			{
				leftPlane = aJsonObject["LeftPlane"].get<float>();
			}

			if (aJsonObject.contains("RightPlane"))
			{
				rightPlane = aJsonObject["RightPlane"].get<float>();
			}

			if (aJsonObject.contains("TopPlane"))
			{
				topPlane = aJsonObject["TopPlane"].get<float>();
			}

			if (aJsonObject.contains("BottomPlane"))
			{
				bottomPlane = aJsonObject["BottomPlane"].get<float>();
			}

			if (aJsonObject.contains("NearPlane"))
			{
				nearPlane = aJsonObject["NearPlane"].get<float>();
			}

			if (aJsonObject.contains("FarPlane"))
			{
				farPlane = aJsonObject["FarPlane"].get<float>();
			}

			InitOrtographicProjection(leftPlane, rightPlane, topPlane, bottomPlane, nearPlane, farPlane);
		}
	}

	return true;
}
