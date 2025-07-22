#pragma once
#include "RHI/RenderHardwareInterface.h"
#include "GraphicsSettings.hpp"

struct PSODescription
{
	std::string name;
	VertexType vertexType;
	std::filesystem::path vsPath;
	std::shared_ptr<Shader> vsShader;
	std::shared_ptr<Shader> gsShader;
	std::shared_ptr<Shader> psShader;
	std::unordered_map<unsigned, std::string> samplerList;
	FillMode fillMode = FillMode::Solid;
	CullMode cullMode = CullMode::Back;
	BlendMode blendMode = BlendMode::None;
	DepthMode depthMode = DepthMode::None;
	bool antiAliasedLine = false;
	bool alphaToCoverage = false;
	bool independentBlend = false;
};

class ResourceVendor
{
public:
	bool Initialize(std::shared_ptr<RenderHardwareInterface> aRHI);

	bool CreateTexture(std::string_view aName, unsigned aWidth, unsigned aHeight,
		RHITextureFormat aFormat, Texture& outTexture, bool aStaging, bool aShaderResource, bool aRenderTarget,
		bool aCpuAccessRead, bool aCpuAccessWrite) const;
	bool LoadTexture(std::string_view aName, const uint8_t* aTextureDataPtr, size_t aTextureDataSize, Texture& outTexture) const;
	bool LoadTexture(const std::filesystem::path& aFilePath, Texture& outTexture) const;

	bool CreateShadowMap(std::string_view aName, unsigned aWidth, unsigned aHeight, Texture& outTexture);
	bool CreateShadowCubemap(std::string_view aName, unsigned aWidth, unsigned aHeight, Texture& outTexture);

	bool LoadShader(const std::filesystem::path& aFilePath, Shader& outShader);
	bool CreatePSO(PipelineStateObject& aPSO, PSODescription& aPSOdesc);

	const bool CompareShaderParameters(const std::filesystem::path& aShaderOnePath, const std::filesystem::path& aShaderTwoPath) const;

	const bool ValidateShaderCombination(
		const std::filesystem::path& aVertexShaderPath,
		const std::filesystem::path& aGeometryShaderPath,
		const std::filesystem::path& aPixelShaderPath) const;

	template <typename VertexType>
	bool CreateVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer) const;
	template <typename VertexType>
	bool CreateDynamicVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer, size_t aMaxVertexCount) const;
	bool CreateIndexBuffer(std::string_view aName, const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIxBuffer, bool aIsDynamic = false);

	Mesh CreatePlanePrimitive() const;
	Mesh CreateCubePrimitive() const;
	Mesh CreateRampPrimitive() const;
private:
	std::shared_ptr<RenderHardwareInterface> myRHI;
};

template<typename VertexType>
inline bool ResourceVendor::CreateVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer) const
{
	return myRHI->CreateVertexBuffer(aName, aVertexList, outVxBuffer);
}

template<typename VertexType>
inline bool ResourceVendor::CreateDynamicVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer, size_t aMaxVertexCount) const
{
	return myRHI->CreateDynamicVertexBuffer(aName, aVertexList, outVxBuffer, aMaxVertexCount);
}