#pragma once
#include "GraphicsEngine.h"

struct ID3D11Buffer;

class DynamicVertexBuffer
{
public:
    DynamicVertexBuffer();
    ~DynamicVertexBuffer();

    template<typename VertexType>
    bool CreateBuffer(std::string_view aName, std::vector<VertexType>& aVertexList, size_t aMaxVertexCount);

    template<typename VertexType>
    bool UpdateVertexBuffer(std::vector<VertexType>& aVertexList);

    const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetVertexBuffer() const { return myVertexBuffer; }
    const size_t GetMaxVertexCount() const { return myMaxVertexCount; }
private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
    size_t myMaxVertexCount = 0;
};

template<typename VertexType>
inline bool DynamicVertexBuffer::CreateBuffer(std::string_view aName, std::vector<VertexType>& aVertexList, size_t aMaxVertexCount)
{
    myMaxVertexCount = aMaxVertexCount;
    return GraphicsEngine::Get().GetResourceVendor().CreateDynamicVertexBuffer(aName, aVertexList, myVertexBuffer, aMaxVertexCount);
}

template<typename VertexType>
inline bool DynamicVertexBuffer::UpdateVertexBuffer(std::vector<VertexType>& aVertexList)
{
    return GraphicsEngine::Get().UpdateDynamicVertexBuffer(aVertexList, *this);
}
