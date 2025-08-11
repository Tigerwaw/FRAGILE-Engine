#pragma once
#include <functional>
#include "GraphicsCommandBase.h"

using GraphicsCommandFunction = std::function<void()>;

class LambdaGraphicsCommand final : public GraphicsCommandBase
{
public:
	GraphicsCommandFunction myLambda;

	LambdaGraphicsCommand(GraphicsCommandFunction&& aLambda) : myLambda(std::move(aLambda))
	{

	}

	void Execute() override
	{
		myLambda();
	}
};

