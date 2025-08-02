#pragma once
#include "ComponentSystem/Component.h"
#include "Objects/Text/Text.h"

class TextComponent : public Component
{
public:
	TextComponent() = default;

	void Start() override;
	void Update() override;

	std::shared_ptr<Text> GetText() { return myText; }

private:
	std::shared_ptr<Text> myText;
};

