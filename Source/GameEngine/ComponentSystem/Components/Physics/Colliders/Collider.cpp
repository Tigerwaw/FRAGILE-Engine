#include "Enginepch.h"
#include "Collider.h"

void Collider::SetOnCollisionEnterResponse(const std::function<void()>& aCallback)
{
	myOnCollisionEnterResponse = aCallback;
}

void Collider::SetOnCollisionStayResponse(const std::function<void()>& aCallback)
{
	myOnCollisionStayResponse = aCallback;
}

void Collider::SetOnCollisionExitResponse(const std::function<void()>& aCallback)
{
	myOnCollisionExitResponse = aCallback;
}

void Collider::SetOnTriggerEnterResponse(const std::function<void()>& aCallback)
{
	myOnTriggerEnterResponse = aCallback;
}

void Collider::SetOnTriggerStayResponse(const std::function<void()>& aCallback)
{
	myOnTriggerStayResponse = aCallback;
}

void Collider::SetOnTriggerExitResponse(const std::function<void()>& aCallback)
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

void Collider::OnCollisionEnter()
{
	if (myOnCollisionEnterResponse)
	{
		myOnCollisionEnterResponse();
	}
}

void Collider::OnCollisionStay()
{
	myIsOverlappingDebug = true;

	if (myOnCollisionStayResponse)
	{
		myOnCollisionStayResponse();
	}
}

void Collider::OnCollisionExit()
{
	myIsOverlappingDebug = false;

	if (myOnCollisionExitResponse)
	{
		myOnCollisionExitResponse();
	}
}

void Collider::OnTriggerEnter()
{
	myIsOverlappingDebug = true;

	if (myOnTriggerEnterResponse)
	{
		myOnTriggerEnterResponse();
	}
}

void Collider::OnTriggerStay()
{
	if (myOnTriggerStayResponse)
	{
		myOnTriggerStayResponse();
	}
}

void Collider::OnTriggerExit()
{
	myIsOverlappingDebug = false;

	if (myOnTriggerExitResponse)
	{
		myOnTriggerExitResponse();
	}
}
