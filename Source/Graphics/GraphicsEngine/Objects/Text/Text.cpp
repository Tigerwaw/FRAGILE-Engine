#include "GraphicsEngine.pch.h"
#include "Text.h"
#include "Objects/DynamicVertexBuffer.h"

Text::Text()
{
	InitializeBuffers();
}

Text::Text(const std::string& aTextContent, std::shared_ptr<Font> aFont)
{
	InitializeBuffers();
	SetTextContent(aTextContent);
	SetFont(aFont);
}

void Text::InitializeBuffers()
{
	myTextData.indices.resize(GraphicsSettings::TEXT_BUFFER_INDEX_COUNT);

	myTextData.vertexBuffer = std::make_shared<DynamicVertexBuffer>();
	myTextData.vertexBuffer->CreateBuffer("Text Vertex Buffer", myTextData.vertices, GraphicsSettings::TEXT_BUFFER_VERTEX_COUNT);
	GraphicsEngine::Get().GetResourceVendor().CreateIndexBuffer("Text Index Buffer", myTextData.indices, myTextData.indexBuffer, true);
}

void Text::UpdateBuffers()
{
	myTextData.vertexBuffer->UpdateVertexBuffer(myTextData.vertices);
	GraphicsEngine::Get().UpdateDynamicIndexBuffer(myTextData.indices, myTextData.indexBuffer);
}

void Text::SetTextContent(const std::string& aTextContent)
{
	myTextData.vertices.clear();
	myTextData.indices.clear();

	myTextContent = aTextContent;
	float X = 0;
	float scalarOffset = 0.65f;

	for (char c : myTextContent)
	{
		Font::Glyph& glyph = myFont->operator[](c);

		const float fontSize = static_cast<float>(myFont->Atlas.Size);
		const float charAdvance = glyph.Advance * fontSize;
		const unsigned currentVertexCount = static_cast<unsigned>(myTextData.vertices.size());
		Math::Vector4f bounds = glyph.UVBounds;
		Math::Vector4f offsets = glyph.PlaneBounds;

		offsets.x = glyph.PlaneBounds.x * charAdvance;
		offsets.z = glyph.PlaneBounds.z * fontSize * scalarOffset;
		offsets.y = glyph.PlaneBounds.y * charAdvance;
		offsets.w = glyph.PlaneBounds.w * fontSize;

		if (abs(offsets.w) > 0.005f)
		{
			offsets.w += myFont->Atlas.Descender * fontSize;
		}

		myTextData.vertices.emplace_back(X + offsets.x, offsets.y, bounds.x, 1 - bounds.y);
		myTextData.vertices.emplace_back(X + offsets.x, offsets.w, bounds.x, 1 - bounds.w);
		myTextData.vertices.emplace_back(X + offsets.z, offsets.y, bounds.z, 1 - bounds.y);
		myTextData.vertices.emplace_back(X + offsets.z, offsets.w, bounds.z, 1 - bounds.w);

		X += charAdvance + scalarOffset;

		myTextData.indices.emplace_back(currentVertexCount);
		myTextData.indices.emplace_back(currentVertexCount + 1);
		myTextData.indices.emplace_back(currentVertexCount + 2);
		myTextData.indices.emplace_back(currentVertexCount + 1);
		myTextData.indices.emplace_back(currentVertexCount + 3);
		myTextData.indices.emplace_back(currentVertexCount + 2);
	}

	myTextData.numVertices = static_cast<unsigned>(myTextData.vertices.size());
	myTextData.numIndices = static_cast<unsigned>(myTextData.indices.size());

	UpdateBuffers();
}

void Text::SetFont(std::shared_ptr<Font> aFont)
{
	myFont = aFont;
	UpdateBuffers();
}