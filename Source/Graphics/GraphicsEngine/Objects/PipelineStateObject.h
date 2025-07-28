#pragma once
#include <memory>
#include <unordered_map>
#include <array>

#include "GraphicsSettings.hpp"

struct ID3D11BlendState;
struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11InputLayout;
struct ID3D11SamplerState;

class Shader;
class Texture;

struct PipelineStateObject
{
	PipelineStateObject();
	~PipelineStateObject();

	std::shared_ptr<PipelineStateObject> CreateInstance() const;

	void SetShader(ShaderType aShaderType, std::shared_ptr<Shader> aShader);
	std::shared_ptr<Shader> GetShader(ShaderType aShaderType) const;

	Microsoft::WRL::ComPtr<ID3D11BlendState> BlendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> RasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout;
	std::array<std::shared_ptr<Shader>, sizeof(ShaderType::COUNT)> Shaders;
	std::unordered_map<unsigned, Microsoft::WRL::ComPtr<ID3D11SamplerState>> SamplerStates;
	std::unordered_map<unsigned, std::shared_ptr<Texture>> TextureResources;

	unsigned VertexStride = 0;
};

