#pragma once
#include "Font.h"
#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"

#include "Objects/Vertices/TextVertex.h"

struct ID3D11Buffer;
class DynamicVertexBuffer;

class Text
{
public:
    Text();
    Text(const std::string& aTextContent, std::shared_ptr<Font> aFont);

    struct TextData
    {
        std::vector<TextVertex> vertices;
        std::vector<unsigned> indices;
        unsigned numVertices = 0;
        unsigned numIndices = 0;
        std::shared_ptr<DynamicVertexBuffer> vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
    };


    void SetTextContent(const std::string& aTextContent);
    void SetFont(std::shared_ptr<Font> aFont);

    const std::shared_ptr<Texture> GetTexture() const { return myFont->Texture; }
    const TextData& GetTextData() const { return myTextData; }
private:
    void InitializeBuffers();
    void UpdateBuffers();

    TextData myTextData;
    std::shared_ptr<Font> myFont;
    std::string myTextContent;
};