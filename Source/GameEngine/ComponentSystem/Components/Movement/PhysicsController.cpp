#include "Enginepch.h"

#include "PhysicsController.h"
#include "Engine.h"
#include "Time/Timer.h"
#include "Input/InputHandler.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"
#include "ComponentSystem/Components/Physics/Rigidbody.h"

PhysicsController::PhysicsController(float aMoveSpeed, float aRotSpeed)
{
	myMoveSpeed = aMoveSpeed;
	myMoveSpeedMultiplier = 1.0f;
	myRotSpeed = aRotSpeed;
}

void PhysicsController::Start()
{
	InputHandler& inputHandler = Engine::Get().GetInputHandler();
	inputHandler.RegisterBinaryAction("ObjectSpeedUp", Keys::SHIFT, GenericInput::ActionType::Clicked);
	inputHandler.RegisterBinaryAction("ObjectSpeedDown", Keys::SHIFT, GenericInput::ActionType::Released);
	inputHandler.RegisterAnalogAction("ObjectXMovement", Keys::A, Keys::D);
	inputHandler.RegisterAnalogAction("ObjectYMovement", Keys::CONTROL, Keys::SPACE);
	inputHandler.RegisterAnalogAction("ObjectZMovement", Keys::S, Keys::W);
	inputHandler.RegisterAnalogAction("ObjectYRotation", Keys::Q, Keys::E);
}

void PhysicsController::Update()
{
	InputHandler& inputHandler = Engine::Get().GetInputHandler();

	if (inputHandler.GetBinaryAction("ObjectSpeedUp"))
	{
		myMoveSpeedMultiplier = 2.0f;
	}
	else if (inputHandler.GetBinaryAction("ObjectSpeedDown"))
	{
		myMoveSpeedMultiplier = 1.0f;
	}

	auto transform = gameObject->GetComponent<Transform>();
	Math::Vector3f inputDelta;
	inputDelta += transform->GetRightVector() * inputHandler.GetAnalogAction("ObjectXMovement");
	inputDelta += transform->GetUpVector() * inputHandler.GetAnalogAction("ObjectYMovement");
	inputDelta += transform->GetForwardVector() * inputHandler.GetAnalogAction("ObjectZMovement");

	if (inputDelta.LengthSqr() > 1.0f)
	{
		inputDelta.Normalize();
	}


	//Math::Vector3f rotationDelta;
	//rotationDelta.y = inputHandler.GetAnalogAction("ObjectYRotation");

	//rotationDelta *= myRotSpeed;

	auto rb = gameObject->GetComponent<Rigidbody>();
	rb->ApplyForce(inputDelta * myMoveSpeed * myMoveSpeedMultiplier);
	/*gameObject->GetComponent<Transform>()->AddRotation(rotationDelta);
	gameObject->GetComponent<Transform>()->SetTranslation(gameObject->GetComponent<Transform>()->GetTranslation() + inputDelta * myMoveSpeed * myMoveSpeedMultiplier * deltaTime);*/
}
