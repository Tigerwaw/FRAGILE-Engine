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

bool Camera::GetViewcullingIntersection(const Math::AABB3D<float>& aObjectAABB)
{
	return Math::IntersectionBetweenPlaneVolumeAABB(GetFrustumPlaneVolume(), aObjectAABB);
}

bool Camera::GetViewcullingIntersection(std::shared_ptr<Transform> aObjectTransform, const Math::AABB3D<float>& aObjectAABB)
{
	return Math::IntersectionBetweenPlaneVolumeAABB(GetFrustumPlaneVolume(aObjectTransform->GetWorldMatrixInverse()), aObjectAABB);
}

bool Camera::GetViewcullingIntersection(std::shared_ptr<Transform> aObjectTransform, const std::array<Math::Vector3f, 8>& aLightFrustum)
{
	auto planeVolume = GetFrustumPlaneVolume(aObjectTransform->GetWorldMatrixInverse());

	int cornersInside = 0;
	for (auto& corner : aLightFrustum)
	{
		if (planeVolume.IsInside(corner))
			++cornersInside;
	}

	return cornersInside > 0;
}

float Camera::GetBoundingBoxScreenPercentage(std::shared_ptr<Transform> aObjectTransform, const Math::AABB3D<float>& aObjectAABB)
{
	Math::Matrix4x4f camWorldInv = gameObject->GetComponent<Transform>()->GetWorldMatrixInverse();
	Math::AABB3D<float> aabbWorldSpace = aObjectAABB.GetAABBinNewSpace(aObjectTransform->GetWorldMatrix());
	Math::AABB3D<float> aabbViewSpace = aabbWorldSpace.GetAABBinNewSpace(camWorldInv);

	Math::Vector3f newMin = { FLT_MAX, FLT_MAX, FLT_MAX };
	Math::Vector3f newMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

	for (Math::Vector3f corner : aabbViewSpace.GetCorners())
	{
		Math::Vector4f newCorner = ToVector4(corner, 1.0f) * myProjectionMatrix;
		newCorner.x /= newCorner.w;
		newCorner.y /= newCorner.w;
		newCorner.z /= newCorner.w;
		corner = ToVector3(newCorner);

		newMin.x = std::fminf(corner.x, newMin.x);
		newMax.x = std::fmaxf(corner.x, newMax.x);
		newMin.y = std::fminf(corner.y, newMin.y);
		newMax.y = std::fmaxf(corner.y, newMax.y);
		newMin.z = std::fminf(corner.z, newMin.z);
		newMax.z = std::fmaxf(corner.z, newMax.z);
	}

	newMin.x = ((newMin.x + 1.0f) * 0.5f) * myViewportDimensions.x;
	newMin.y = ((newMin.y + 1.0f) * 0.5f) * myViewportDimensions.y;

	Math::Vector2f objectSides;
	objectSides.x = newMax.x - newMin.x;
	objectSides.y = newMax.y - newMin.y;

	float objectArea = objectSides.x * objectSides.y;
	float viewportArea = myViewportDimensions.x * myViewportDimensions.y;
	float screenPercentage = objectArea / viewportArea;
	return screenPercentage;

	// Move bounding box into world space
	// Move bounding box into camera space
	// Move bounding box into screen space
	// Get 2D extents (min x/y, max x/y of all 8 points)
	// Compare volumes of 2D bounding box and viewport
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
