#include "Enginepch.h"

#include "Scene.h"
#include "GraphicsEngine.h"
#include "Objects/Sprite.h"
#include "Objects/Material.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"
#include "ComponentSystem/Components/Graphics/Model.h"
#include "ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "ComponentSystem/Components/Graphics/Camera.h"
#include "ComponentSystem/Components/Lights/AmbientLight.h"
#include "ComponentSystem/Components/Lights/DirectionalLight.h"
#include "ComponentSystem/Components/Lights/PointLight.h"
#include "ComponentSystem/Components/Lights/SpotLight.h"

#include "RenderAssembler/RenderAssembler.h"
#include "Engine.h"
#include "DebugDrawer/DebugDrawer.h"
#include "AssetManager.h"

#include "Math/Vector.hpp"

void Scene::Update()
{
	PIXScopedEvent(PIX_COLOR_INDEX(7), "Update GameObjects in Scene");
	myActiveGameObjectAmount = 0;

	for (auto& gameObject : myGameObjectsToDestroy)
	{
		if (auto goTransform = gameObject->GetComponent<Transform>())
		{
			DestroyHierarchy(goTransform.get());
		}
		else
		{
			DestroyInternal(gameObject.get());
		}
	}

	myGameObjectsToDestroy.clear();

	for (auto& gameObject : myGameObjects)
	{
		if (gameObject->GetActive())
		{
			gameObject->Update();
			myActiveGameObjectAmount++;
		}
	}
}

std::shared_ptr<GameObject> Scene::FindGameObjectByName(const std::string& aName)
{
	if (auto go = std::find_if(myGameObjects.begin(), myGameObjects.end(), [aName](const std::shared_ptr<GameObject>& object) { return object->GetName() == aName; }); *go != nullptr)
	{
		return *go;
	}

	LOG(LogScene, Warning, "Could not find game object with name {} in the scene!", aName);
	return std::shared_ptr<GameObject>();
}

std::shared_ptr<GameObject> Scene::FindGameObjectByID(const unsigned aID)
{
	if (auto go = std::find_if(myGameObjects.begin(), myGameObjects.end(), [aID](const std::shared_ptr<GameObject>& object) { return object->GetID() == aID; }); go != myGameObjects.end() && *go != nullptr)
	{
		return *go;
	}

	LOG(LogScene, Warning, "Could not find game object with ID {} in the scene!", aID);
	return std::shared_ptr<GameObject>();
}

std::shared_ptr<GameObject> Scene::FindGameObjectByNetworkID(const unsigned aNetworkID)
{
	if (auto go = std::find_if(myGameObjects.begin(), myGameObjects.end(), [aNetworkID](const std::shared_ptr<GameObject>& object) { return object->GetNetworkID() == aNetworkID; }); go != myGameObjects.end() && *go != nullptr)
	{
		return *go;
	}

	LOG(LogScene, Warning, "Could not find game object with network ID {} in the scene!", aNetworkID);
	return std::shared_ptr<GameObject>();
}

void Scene::Instantiate(std::shared_ptr<GameObject> aGameObject)
{
	if (!aGameObject)
	{
		LOG(LogScene, Warning, "Tried to instantiate a non-existing gameobject!");
		return;
	}

	UpdateBoundingBox(aGameObject);

	aGameObject->SetID(GetFreeID());
	myGameObjects.emplace_back(aGameObject);

	LOG(LogScene, Log, "Created GameObject {} with ID {}!", aGameObject->GetName(), aGameObject->GetID());
}

void Scene::Destroy(std::shared_ptr<GameObject> aGameObject)
{
	if (!aGameObject)
	{
		LOG(LogScene, Warning, "Tried to destroy a non-existing gameobject!");
		return;
	}

	for (size_t i = 0; i < myGameObjects.size(); i++)
	{
		if (myGameObjects[i].get() == aGameObject.get())
		{
			myGameObjectsToDestroy.push_back(aGameObject);
			return;
		}
	}

	LOG(LogScene, Warning, "Could not find GameObject {} with ID {} in scene!", aGameObject->GetName(), aGameObject->GetID());
}

void Scene::DestroyInternal(GameObject* aGameObject)
{
	for (size_t i = 0; i < myGameObjects.size(); i++)
	{
		if (myGameObjects[i].get() == aGameObject)
		{
			LOG(LogScene, Log, "Destroyed GameObject {} with ID {}!", aGameObject->GetName(), aGameObject->GetID());
			unsigned id = aGameObject->GetID();
			myGameObjects.erase(myGameObjects.begin() + i);
			myFreeIDs.Push(id);
			return;
		}
	}
}

void Scene::DestroyHierarchy(Transform* aTransform)
{
	for (auto& child : aTransform->GetChildren())
	{
		DestroyHierarchy(child);
	}

	DestroyInternal(aTransform->gameObject);
}

void Scene::UpdateBoundingBox(std::shared_ptr<GameObject> aGameObject)
{
	if (aGameObject->GetComponent<Transform>())
	{
		Math::Vector3f bbMin = myBoundingBox.GetMin();
		Math::Vector3f bbMax = myBoundingBox.GetMax();
		std::shared_ptr<Transform> objectTransform = aGameObject->GetComponent<Transform>();

		if (aGameObject->GetComponent<Model>())
		{
			auto& corners = aGameObject->GetComponent<Model>()->GetBoundingBox().GetCorners();

			for (Math::Vector3f corner : corners)
			{
				corner = Math::ToVector3(Math::ToVector4(corner, 1.0f) * objectTransform->GetWorldMatrix());

				bbMin.x = std::fminf(corner.x, bbMin.x);
				bbMax.x = std::fmaxf(corner.x, bbMax.x);
				bbMin.y = std::fminf(corner.y, bbMin.y);
				bbMax.y = std::fmaxf(corner.y, bbMax.y);
				bbMin.z = std::fminf(corner.z, bbMin.z);
				bbMax.z = std::fmaxf(corner.z, bbMax.z);
			}
		}
		else if (aGameObject->GetComponent<AnimatedModel>())
		{
			auto& corners = aGameObject->GetComponent<AnimatedModel>()->GetBoundingBox().GetCorners();
			for (Math::Vector3f corner : corners)
			{
				corner = Math::ToVector3(Math::ToVector4(corner, 1.0f) * objectTransform->GetWorldMatrix());

				bbMin.x = std::fminf(corner.x, bbMin.x);
				bbMax.x = std::fmaxf(corner.x, bbMax.x);
				bbMin.y = std::fminf(corner.y, bbMin.y);
				bbMax.y = std::fmaxf(corner.y, bbMax.y);
				bbMin.z = std::fminf(corner.z, bbMin.z);
				bbMax.z = std::fmaxf(corner.z, bbMax.z);
			}
		}
		else
		{
			Math::Vector3f point = objectTransform->GetTranslation(true);

			bbMin.x = std::fminf(point.x, bbMin.x);
			bbMax.x = std::fmaxf(point.x, bbMax.x);
			bbMin.y = std::fminf(point.y, bbMin.y);
			bbMax.y = std::fmaxf(point.y, bbMax.y);
			bbMin.z = std::fminf(point.z, bbMin.z);
			bbMax.z = std::fmaxf(point.z, bbMax.z);
		}

		myBoundingBox.InitWithMinAndMax(bbMin, bbMax);
	}
}

unsigned Scene::GetFreeID()
{
	if (!myFreeIDs.IsEmpty())
		return myFreeIDs.Pop();

	return ++myCurrentGameObjectID;
}
