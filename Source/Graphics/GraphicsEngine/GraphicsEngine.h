#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <wrl.h>
#include "RHI/RenderHardwareInterface.h"
#include "Objects/PipelineStateObject.h"
#include "GraphicsCommands/GraphicsCommandList.h"
#include "Objects/ConstantBuffers/ConstantBuffer.h"
#include "GraphicsSettings.hpp"
#include "PostProcessingSettings.h"
#include "Drawer.h"
#include "ResourceVendor.h"

class Shader;
class Mesh;
class Text;
class Sprite;
class DynamicVertexBuffer;
class GraphicsCommandList;
class GraphicsCommandBase;
class GBuffer;
class ParticleEmitter;
class TrailEmitter;

struct ID3D11InputLayout;

struct Vertex;

class GraphicsEngine
{
	friend class Drawer;

public:
	static GraphicsEngine& Get()
	{
		static GraphicsEngine myInstance;
		return myInstance;
	}
	bool Initialize(HWND aWindowHandle, Math::Vector2f aResolution, const std::filesystem::path& aContentRoot);

#ifndef _RETAIL
	bool InitializeImGui();
#endif

	PostProcessingSettings& GetPostProcessingSettings() { return *myPostProcessingSettings; }
	Drawer& GetDrawer() { return *myDrawer; }
	ResourceVendor& GetResourceVendor() { return *myResourceVendor; }

	void BeginFrame();
	void RenderFrame();
	void EndFrame();

	void SetResolution(float aNewWidth, float aNewHeight);
	void SetWindowSize(float aNewWidth, float aNewHeight);
	void MaximizeWindowSize();

	template<typename BufferData>
	bool UpdateAndSetConstantBuffer(const char* aBufferType, const BufferData& aDataBlock);

	template<typename BufferData>
	bool UpdateAndSetConstantBuffer(ConstantBufferType aBufferType, const BufferData& aDataBlock);

	void ChangePipelineState(const std::shared_ptr<PipelineStateObject> aNewPSO);

	bool SetTextureResource_PS(unsigned aSlot, Texture& aTexture);
	bool SetTextureResource_VS(unsigned aSlot, Texture& aTexture);
	bool ClearTextureResource_PS(unsigned aSlot);
	bool ClearTextureResource_VS(unsigned aSlot);

	void SetRenderTarget(std::shared_ptr<Texture> aRenderTarget, std::shared_ptr<Texture> aDepthStencil, bool aClearRenderTarget = true, bool aClearDepthStencil = true);
	void SetRenderTargets(const std::vector<std::shared_ptr<Texture>>& aRenderTargets, std::shared_ptr<Texture> aDepthStencil, bool aClearRenderTarget = true, bool aClearDepthStencil = true);

	template <typename VertexType>
	bool UpdateDynamicVertexBuffer(const std::vector<VertexType>& aVertexList, DynamicVertexBuffer& outVxBuffer) const;
	bool UpdateDynamicIndexBuffer(const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIxBuffer);

	void BeginEvent(std::string_view aEvent) const { myRHI->BeginEvent(aEvent); }
	void EndEvent() const { myRHI->EndEvent(); }
	void SetMarker(std::string_view aMarker) const { myRHI->SetMarker(aMarker); }

	std::shared_ptr<Texture> GetBackBuffer() { return myRHI->GetBackBuffer(); }
	std::shared_ptr<Texture> GetDepthBuffer() { return myRHI->GetDepthBuffer(); }
	std::shared_ptr<Texture> GetIntermediateTexture(IntermediateTexture aIntermediateTexture);
	GBuffer& GetGBuffer() { return *myGBuffer; }

	GraphicsCommandList& GetGraphicsCommandList() const { return *myCommandList; }
	std::shared_ptr<PipelineStateObject> GetDefaultPSO() const { return myDefaultPSO; }
	bool HasPSO(const char* aPSOName) const;
	std::shared_ptr<PipelineStateObject> GetPSO(const char* aPSOName) const;
	std::shared_ptr<PipelineStateObject> GetPSO(PSOType aPSOType) const;
	std::shared_ptr<PipelineStateObject> RegisterPSO(const char* aPSOName, std::shared_ptr<PipelineStateObject> aPSO);

	bool IsShaderCached(const std::filesystem::path& aShaderFilename) const;
	std::shared_ptr<Shader> GetCachedShader(const std::filesystem::path& aShaderFilename);
	const ShaderInfo& GetCachedShaderInfo(const std::filesystem::path& aShaderFilename);
	void CacheShader(const std::filesystem::path& aShaderFilename, std::shared_ptr<Shader> aShader);

	std::shared_ptr<Texture> GetBlueNoiseTexture() const { return myBlueNoiseTexture; }
	std::shared_ptr<Texture> GetPerlinNoiseTexture() const { return myPerlinNoiseTexture; }

	std::shared_ptr<Texture> GetDefaultAlbedoTexture() const { return myDefaultAlbedoTexture; }
	std::shared_ptr<Texture> GetDefaultNormalTexture() const { return myDefaultNormalTexture; }
	std::shared_ptr<Texture> GetDefaultARMTexture() const { return myDefaultARMTexture; }
	std::shared_ptr<Texture> GetDefaultFXTexture() const { return myDefaultFXTexture; }

	std::shared_ptr<Material> GetDefaultMaterial() const { return myDefaultMaterial; }

	std::shared_ptr<Mesh> GetPlanePrimitive() const { return myPlanePrimitive; }
	std::shared_ptr<Mesh> GetCubePrimitive() const { return myCubePrimitive; }
	std::shared_ptr<Mesh> GetRampPrimitive() const { return myRampPrimitive; }

	const unsigned GetDrawcallAmount() const { return myLastFrameDrawcallAmount; }

	DebugRenderMode CurrentDebugRenderMode = DebugRenderMode::None;

private:
	GraphicsEngine();
	~GraphicsEngine();

	bool CreateMeshPrimitives();
	bool CreateDefaultPSOs(const std::filesystem::path& aContentRoot);
	bool CreateDefaultTextures(const std::filesystem::path& aContentRoot);
	bool CreateConstantBuffers();

	std::shared_ptr<RenderHardwareInterface> myRHI;
	std::unique_ptr<PostProcessingSettings> myPostProcessingSettings;
	std::unique_ptr<Drawer> myDrawer;
	std::unique_ptr<ResourceVendor> myResourceVendor;

	std::unordered_map<std::string, ConstantBuffer> myConstantBuffers;
	std::unordered_map<std::string, std::shared_ptr<PipelineStateObject>> myPSOs;
	std::unordered_map<std::string, std::shared_ptr<Shader>> myCachedShaders;
	std::unordered_map<std::string, ShaderInfo> myCachedShaderInfo;
	
	std::shared_ptr<PipelineStateObject> myCurrentPSO;
	std::shared_ptr<PipelineStateObject> myDefaultPSO;

	std::shared_ptr<Texture> myBRDFLUTTexture;
	std::shared_ptr<Texture> myBlueNoiseTexture;
	std::shared_ptr<Texture> myPerlinNoiseTexture;
	
	std::unique_ptr<GBuffer> myGBuffer;
	std::unique_ptr<GraphicsCommandList> myCommandList;

	std::shared_ptr<Texture> myDefaultAlbedoTexture;
	std::shared_ptr<Texture> myDefaultNormalTexture;
	std::shared_ptr<Texture> myDefaultARMTexture;
	std::shared_ptr<Texture> myDefaultFXTexture;

	std::shared_ptr<Material> myDefaultMaterial;

	std::shared_ptr<Mesh> myPlanePrimitive;
	std::shared_ptr<Mesh> myCubePrimitive;
	std::shared_ptr<Mesh> myRampPrimitive;

	unsigned myDrawcallAmount = 0;
	unsigned myLastFrameDrawcallAmount = 0;
};

template<typename BufferData>
bool GraphicsEngine::UpdateAndSetConstantBuffer(const char* aBufferType, const BufferData& aDataBlock)
{
	if (!myConstantBuffers.contains(aBufferType))
	{
		return false;
	}

	ConstantBuffer& buffer = myConstantBuffers[aBufferType];
	if (!myRHI->UpdateConstantBuffer(buffer, aDataBlock))
	{
		return false;
	}

	myRHI->SetConstantBuffer(buffer);
	return true;
}

template<typename BufferData>
bool GraphicsEngine::UpdateAndSetConstantBuffer(ConstantBufferType aBufferType, const BufferData& aDataBlock)
{
	return UpdateAndSetConstantBuffer(ConstantBufferName(aBufferType), aDataBlock);
}

template<typename VertexType>
inline bool GraphicsEngine::UpdateDynamicVertexBuffer(const std::vector<VertexType>& aVertexList, DynamicVertexBuffer& outVxBuffer) const
{
	return myRHI->UpdateDynamicVertexBuffer(aVertexList, outVxBuffer);
}
