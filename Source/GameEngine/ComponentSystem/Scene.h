#pragma once
#include "Math/AABB3D.hpp"
#include "CommonUtilities/StaticStack.hpp"

class GameObject;
class Camera;
class Transform;
struct PipelineStateObject;

class Scene final
{
public:
	friend class SceneHandler;
	friend class SceneLoader;
	friend class RenderAssembler;
	friend class CollisionHandler;

	void Update();
	std::shared_ptr<GameObject> FindGameObjectByName(const std::string& aName);
	std::shared_ptr<GameObject> FindGameObjectByID(const unsigned aID);
	std::shared_ptr<GameObject> FindGameObjectByNetworkID(const unsigned aNetworkID);

	void Instantiate(std::shared_ptr<GameObject> aGameObject);
	void Destroy(std::shared_ptr<GameObject> aGameObject);

	const unsigned GetObjectAmount() const { return static_cast<unsigned>(myGameObjects.size()); }
	const unsigned GetActiveObjectAmount() const { return myActiveGameObjectAmount; }

	void SetActive(bool aIsActive) { myIsActive = aIsActive; }
	const bool GetActive() const { return myIsActive; }

private:
	void DestroyInternal(GameObject* aGameObject);
	void DestroyHierarchy(Transform* aTransform);
	void UpdateBoundingBox(std::shared_ptr<GameObject> aGameObject);
	unsigned GetFreeID();

	std::vector<std::shared_ptr<GameObject>> myGameObjects;
	std::vector<std::shared_ptr<GameObject>> myGameObjectsToDestroy;
	unsigned myActiveGameObjectAmount = 0;
	bool myIsActive = false;
	Math::AABB3D<float> myBoundingBox;

	unsigned myCurrentGameObjectID = 0;
	Utilities::StaticStack<unsigned, 1000> myFreeIDs;
};
