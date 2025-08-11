#pragma once

class GraphicsCommandBase
{
public:
	virtual ~GraphicsCommandBase() {};
	virtual void Execute() = 0;

	GraphicsCommandBase* Next = nullptr;
};

