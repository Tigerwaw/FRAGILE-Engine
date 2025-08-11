#pragma once
#include <memory>
#include "GraphicsCommands/GraphicsCommandBase.h"

class Text;

class RenderText : public GraphicsCommandBase
{
public:
	RenderText(const std::shared_ptr<Text> aText, const Math::Matrix4x4f& aTransform);
	void Execute() override;
private:
	std::shared_ptr<Text> myText;
	Math::Matrix4x4f myTransform;
};