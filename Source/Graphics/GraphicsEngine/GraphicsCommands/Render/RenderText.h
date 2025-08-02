#pragma once
#include <memory>
#include "GraphicsCommands/GraphicsCommandBase.h"

class Text;

class RenderText : public GraphicsCommandBase
{
public:
	struct TextData
	{
		std::shared_ptr<Text> text;
		Math::Matrix4x4f transform;
	};

	RenderText(const TextData& aTextData);
	RenderText(TextData&& aTextData);
	void Execute() override;
	void Destroy() override;
private:
	TextData myData;
};