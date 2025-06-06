#pragma once
#include "Math/Vector.hpp"


class PollingStation
{
public:
	static PollingStation& Get()
	{
		static PollingStation instance;
		return instance;
	}
private:
	PollingStation();
	~PollingStation();
	PollingStation(PollingStation const&) = delete;
	void operator=(PollingStation const&) = delete;
	static PollingStation* myInstance;

public:
	void Update();
	void AddWatchedActor(std::shared_ptr<GameObject> aGameObject);
	void SetWanderer(std::shared_ptr<GameObject> aGameObject);

	const std::vector<std::shared_ptr<GameObject>>& GetOtherActors() { return myWatchedActors; }
	const std::vector<Math::Vector3f> GetOtherActorPositions() const;
	const Math::Vector3f GetWandererPosition() const;
private:
	std::vector<std::shared_ptr<GameObject>> myWatchedActors;
	std::shared_ptr<GameObject> myWanderer;
};