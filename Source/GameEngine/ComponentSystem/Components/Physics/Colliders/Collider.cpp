#include "Enginepch.h"
#include "Collider.h"

void Collider::SetOnCollisionEnterResponse(const std::function<void(CollisionInfo)>& aCallback)
{
	myOnCollisionEnterResponse = aCallback;
}

void Collider::SetOnCollisionStayResponse(const std::function<void(CollisionInfo)>& aCallback)
{
	myOnCollisionStayResponse = aCallback;
}

void Collider::SetOnCollisionExitResponse(const std::function<void(CollisionInfo)>& aCallback)
{
	myOnCollisionExitResponse = aCallback;
}

void Collider::SetOnTriggerEnterResponse(const std::function<void(CollisionInfo)>& aCallback)
{
	myOnTriggerEnterResponse = aCallback;
}

void Collider::SetOnTriggerStayResponse(const std::function<void(CollisionInfo)>& aCallback)
{
	myOnTriggerStayResponse = aCallback;
}

void Collider::SetOnTriggerExitResponse(const std::function<void(CollisionInfo)>& aCallback)
{
	myOnTriggerExitResponse = aCallback;
}

bool Collider::Serialize(nl::json&)
{
	return false;
}

bool Collider::Deserialize(nl::json& aJsonObject)
{
	if (aJsonObject.contains("IsTrigger"))
	{
		SetIsTrigger(aJsonObject["IsTrigger"].get<bool>());
	}
	
	return true;
}

void Collider::OnCollisionEnter(CollisionInfo aCollision)
{
	if (myOnCollisionEnterResponse)
	{
		myOnCollisionEnterResponse(aCollision);
	}
}

void Collider::OnCollisionStay(CollisionInfo aCollision)
{
	myIsOverlappingDebug = true;

	if (myOnCollisionStayResponse)
	{
		myOnCollisionStayResponse(aCollision);
	}
}

void Collider::OnCollisionExit(CollisionInfo aCollision)
{
	myIsOverlappingDebug = false;

	if (myOnCollisionExitResponse)
	{
		myOnCollisionExitResponse(aCollision);
	}
}

void Collider::OnTriggerEnter(CollisionInfo aCollision)
{
	if (myOnTriggerEnterResponse)
	{
		myOnTriggerEnterResponse(aCollision);
	}
}

void Collider::OnTriggerStay(CollisionInfo aCollision)
{
	myIsOverlappingDebug = true;

	if (myOnTriggerStayResponse)
	{
		myOnTriggerStayResponse(aCollision);
	}
}

void Collider::OnTriggerExit(CollisionInfo aCollision)
{
	myIsOverlappingDebug = false;

	if (myOnTriggerExitResponse)
	{
		myOnTriggerExitResponse(aCollision);
	}
}
