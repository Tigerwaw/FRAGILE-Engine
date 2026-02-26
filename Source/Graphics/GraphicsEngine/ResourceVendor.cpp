#include "GraphicsEngine.pch.h"
#include "ResourceVendor.h"
#include "Objects/Mesh.h"
#include "Objects/Texture.h"
#include "Objects/Material.h"
#include "ShaderReflection/ShaderInfo.h"

#include "Objects/Vertices/Vertex.h"
#include "Objects/Vertices/DebugLineVertex.h"
#include "Objects/Vertices/TextVertex.h"
#include "Objects/Vertices/ParticleVertex.h"
#include "Objects/Vertices/TrailVertex.h"

bool ResourceVendor::Initialize(std::shared_ptr<RenderHardwareInterface> aRHI)
{
	myRHI = aRHI;
	return true;
}

bool ResourceVendor::CreateTexture(std::string_view aName, unsigned aWidth, unsigned aHeight, RHITextureFormat aFormat, Texture& outTexture, bool aStaging, bool aShaderResource, bool aRenderTarget, bool aCpuAccessRead, bool aCpuAccessWrite) const
{
	return myRHI->CreateTexture(aName, aWidth, aHeight, aFormat, outTexture, aStaging, aShaderResource, aRenderTarget, aCpuAccessRead, aCpuAccessWrite);
}

bool ResourceVendor::LoadTexture(std::string_view aName, const uint8_t* aTextureDataPtr, size_t aTextureDataSize, Texture& outTexture) const
{
	return myRHI->LoadTexture(aName, aTextureDataPtr, aTextureDataSize, outTexture);
}

bool ResourceVendor::LoadTexture(const std::filesystem::path& aFilePath, Texture& outTexture) const
{
	std::ifstream file(aFilePath, std::ios::binary);
	file.seekg(0, std::ios::end);
	const std::streamoff fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<uint8_t> fileData;
	fileData.resize(fileSize);
	file.read(reinterpret_cast<char*>(fileData.data()), fileSize);
	file.close();

	return myRHI->LoadTexture(aFilePath.filename().string(), fileData.data(), static_cast<size_t>(fileSize), outTexture);
}

bool ResourceVendor::CreateShadowMap(std::string_view aName, unsigned aWidth, unsigned aHeight, Texture& outTexture)
{
	return myRHI->CreateShadowMap(aName, aWidth, aHeight, outTexture);
}

bool ResourceVendor::CreateShadowCubemap(std::string_view aName, unsigned aWidth, unsigned aHeight, Texture& outTexture)
{
	return myRHI->CreateShadowCubemap(aName, aWidth, aHeight, outTexture);
}

bool ResourceVendor::LoadShader(const std::filesystem::path& aFilePath, Shader& outShader)
{
	return myRHI->LoadShaderFromFilePath(aFilePath.stem().string(), outShader, aFilePath.wstring());
}

bool ResourceVendor::CreatePSO(PipelineStateObject& aPSO, PSODescription& aPSOdesc)
{
	std::vector<VertexElementDesc> inputLayoutDefinition;

	switch (aPSOdesc.vertexType)
	{
	case VertexType::MeshVertex:
	{
		inputLayoutDefinition = Vertex::InputLayoutDefinition;
		aPSO.VertexStride = static_cast<unsigned>(sizeof(Vertex));
		break;
	}
	case VertexType::DebugLineVertex:
	{
		inputLayoutDefinition = DebugLineVertex::InputLayoutDefinition;
		aPSO.VertexStride = static_cast<unsigned>(sizeof(DebugLineVertex));
		break;
	}
	case VertexType::TextVertex:
	{
		inputLayoutDefinition = TextVertex::InputLayoutDefinition;
		aPSO.VertexStride = static_cast<unsigned>(sizeof(TextVertex));
		break;
	}
	case VertexType::ParticleVertex:
	{
		inputLayoutDefinition = ParticleVertex::InputLayoutDefinition;
		aPSO.VertexStride = static_cast<unsigned>(sizeof(ParticleVertex));
		break;
	}
	case VertexType::TrailVertex:
	{
		inputLayoutDefinition = TrailVertex::InputLayoutDefinition;
		aPSO.VertexStride = static_cast<unsigned>(sizeof(TrailVertex));
		break;
	}
	}

	if (!aPSOdesc.vsPath.empty())
	{
		if (myRHI->HasInputLayout(aPSOdesc.vsPath))
		{
			aPSO.InputLayout = myRHI->GetInputLayout(aPSOdesc.vsPath);
		}
		else
		{
			if (!myRHI->CreateInputLayout(aPSOdesc.vsPath.filename().string(), aPSO.InputLayout, inputLayoutDefinition, aPSOdesc.vsPath))
			{
				LOG(LogGraphicsEngine, Error, "Failed to create PSO {}!", aPSOdesc.name);
				return false;
			}
		}
	}

	aPSO.SetShader(ShaderType::Vertex, aPSOdesc.vsShader);
	aPSO.SetShader(ShaderType::Geometry, aPSOdesc.gsShader);
	aPSO.SetShader(ShaderType::Pixel, aPSOdesc.psShader);

	if (!(aPSOdesc.fillMode == FillMode::Solid && aPSOdesc.cullMode == CullMode::Back && aPSOdesc.antiAliasedLine == false))
	{
		D3D11_RASTERIZER_DESC rastDesc = {};
		rastDesc.FillMode = static_cast<D3D11_FILL_MODE>(aPSOdesc.fillMode);
		rastDesc.CullMode = static_cast<D3D11_CULL_MODE>(aPSOdesc.cullMode);
		rastDesc.AntialiasedLineEnable = aPSOdesc.antiAliasedLine;

		if (!myRHI->CreateRasterizerState(aPSOdesc.name + "_Rasterizer", rastDesc, aPSO))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create rasterizer for PSO {}!", aPSOdesc.name);
			return false;
		}
	}


	if (aPSOdesc.blendMode != BlendMode::None)
	{
		CD3D11_BLEND_DESC blendDesc = {};
		blendDesc.AlphaToCoverageEnable = aPSOdesc.alphaToCoverage;
		blendDesc.IndependentBlendEnable = aPSOdesc.independentBlend;

		D3D11_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc = {};
		defaultRenderTargetBlendDesc.BlendEnable = TRUE;
		defaultRenderTargetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		if (aPSOdesc.blendMode == BlendMode::Alpha)
		{
			defaultRenderTargetBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
			defaultRenderTargetBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			defaultRenderTargetBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;

			defaultRenderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
			defaultRenderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_ONE;
			defaultRenderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		}
		else if (aPSOdesc.blendMode == BlendMode::Additive)
		{
			defaultRenderTargetBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
			defaultRenderTargetBlendDesc.DestBlend = D3D11_BLEND_ONE;
			defaultRenderTargetBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;

			defaultRenderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
			defaultRenderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_ONE;
			defaultRenderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_MAX;
		}

		for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		{
			blendDesc.RenderTarget[i] = defaultRenderTargetBlendDesc;
		}

		if (!myRHI->CreateBlendState(aPSOdesc.name + "_BlendState", blendDesc, aPSO))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create blend state for PSO {}!", aPSOdesc.name);
			return false;
		}

		LOG(LogGraphicsEngine, Log, "Successfully created blend state for PSO {}!", aPSOdesc.name);
	}

	if (aPSOdesc.depthMode != DepthMode::None)
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = CD3D11_DEPTH_STENCIL_DESC();
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.StencilEnable = false;

		if (aPSOdesc.depthMode == DepthMode::ReadOnly)
		{
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		}
		else if (aPSOdesc.depthMode == DepthMode::Skybox)
		{
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		}
		
		if (!myRHI->CreateDepthStencilState(aPSOdesc.name + "_DepthStencilState", depthStencilDesc, aPSO))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create depth stencil state for PSO {}!", aPSOdesc.name);
			return false;
		}
	}

	for (auto& sampler : aPSOdesc.samplerList)
	{
		aPSO.SamplerStates[sampler.first] = myRHI->GetSamplerState(sampler.second);
	}

	LOG(LogGraphicsEngine, Log, "Created PSO {}!", aPSOdesc.name);
	return true;
}

const bool ResourceVendor::CompareShaderParameters(const std::filesystem::path& aShaderOnePath, const std::filesystem::path& aShaderTwoPath) const
{
	ShaderInfo shaderOneInfo;
	if (GraphicsEngine::Get().IsShaderCached(aShaderOnePath))
	{
		shaderOneInfo = GraphicsEngine::Get().GetCachedShaderInfo(aShaderOnePath);
	}
	else
	{
		shaderOneInfo = myRHI->GetShaderInfo(aShaderOnePath);
	}

	ShaderInfo shaderTwoInfo;
	if (GraphicsEngine::Get().IsShaderCached(aShaderTwoPath))
	{
		shaderTwoInfo = GraphicsEngine::Get().GetCachedShaderInfo(aShaderTwoPath);
	}
	else
	{
		shaderTwoInfo = myRHI->GetShaderInfo(aShaderTwoPath);
	}

	if (shaderOneInfo.GetOutputParameters().size() != shaderTwoInfo.GetInputParameters().size())
	{
		LOG(LogGraphicsEngine, Error, "Output of shader {} ({} outputs) and input of shader {} ({} inputs) do not match!", aShaderOnePath.stem().string(), shaderOneInfo.GetOutputParameters().size(), aShaderTwoPath.stem().string(), shaderTwoInfo.GetInputParameters().size());
		return false;
	}

	for (size_t i = 0; i < shaderOneInfo.GetOutputParameters().size(); i++)
	{
		auto& outParam = shaderOneInfo.GetOutputParameters()[i];
		auto& inParam = shaderTwoInfo.GetInputParameters()[i];
		if (outParam != inParam)
		{
			LOG(LogGraphicsEngine, Error, "Output of shader {} and input of shader {} do not match! {} != {}", aShaderOnePath.stem().string(), aShaderTwoPath.stem().string(), outParam.SemanticName, inParam.SemanticName);
			return false;
		}
	}

	return true;
}

const bool ResourceVendor::ValidateShaderCombination(const std::filesystem::path& aVertexShaderPath, const std::filesystem::path& aGeometryShaderPath, const std::filesystem::path& aPixelShaderPath) const
{
	bool hasVertexShader = aVertexShaderPath != "";
	bool hasGeometryShader = aGeometryShaderPath != "";
	bool hasPixelShader = aPixelShaderPath != "";

	if (hasVertexShader && hasGeometryShader && hasPixelShader)
	{
		return (CompareShaderParameters(aVertexShaderPath, aGeometryShaderPath) && CompareShaderParameters(aGeometryShaderPath, aPixelShaderPath));
	}
	else if (hasVertexShader && hasPixelShader)
	{
		return CompareShaderParameters(aVertexShaderPath, aPixelShaderPath);
	}
	else if (hasVertexShader && hasGeometryShader)
	{
		return CompareShaderParameters(aVertexShaderPath, aGeometryShaderPath);
	}

	return true;
}

bool ResourceVendor::CreateIndexBuffer(std::string_view aName, const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIxBuffer, bool aIsDynamic)
{
	return myRHI->CreateIndexBuffer(aName, aIndexList, outIxBuffer, aIsDynamic);
}

Mesh ResourceVendor::CreatePlanePrimitive() const
{
	float emptyColor[4][4] = { {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f} };
	float uv[4][4][2] = { {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}}
	};
	float pos[4][4] = {
		{1, 0, 1, 1},
		{1, 0, -1, 1},
		{-1, 0, -1, 1},
		{-1, 0, 1, 1}
	};
	float normals[4][3] = {
		{ 0, 1, 0 },
		{ 0, 1, 0 },
		{ 0, 1, 0 },
		{ 0, 1, 0 }
	};
	float tangents[4][3] = {
		{ 1, 0, 0 },
		{ 1, 0, 0 },
		{ 1, 0, 0 },
		{ 1, 0, 0 }
	};

	std::vector<Vertex> vertexList;
	vertexList.reserve(4);
	vertexList.emplace_back(pos[0], emptyColor, uv[0], normals[0], tangents[0]);
	vertexList.emplace_back(pos[1], emptyColor, uv[1], normals[1], tangents[1]);
	vertexList.emplace_back(pos[2], emptyColor, uv[2], normals[2], tangents[2]);
	vertexList.emplace_back(pos[3], emptyColor, uv[3], normals[3], tangents[3]);

	std::vector<unsigned> indexList = {
		0, 1, 2,
		2, 3, 0,
	};

	Mesh plane;
	Mesh::Submesh submesh;
	submesh.MaterialIndex = 0;
	submesh.AddLOD(0, std::move(vertexList), std::move(indexList));
	plane.AddSubmesh(std::move(submesh));
	plane.InitBoundingBox({ -1.0f, -0.001f, -1.0f }, { 1.0f, 0.001f, 1.0f });
	return plane;
}

Mesh ResourceVendor::CreateCubePrimitive() const
{
	float emptyColor[4][4] = { {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f} };
	float uv[24][4][2] = { {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

						  {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

						  {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

						  {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

						  {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

						  {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
	};

	float pos[24][4] = {
		{1,1,-1, 1}, // BACK
		{1,-1,-1, 1},
		{-1,-1,-1, 1},
		{-1,1,-1, 1},

		{1,1,1, 1}, // FRONT
		{-1,1,1, 1},
		{-1,-1,1, 1},
		{1,-1,1, 1},

		{1,1,-1, 1}, // RIGHT
		{1,1,1, 1},
		{1,-1,1, 1},
		{1,-1,-1, 1},

		{1,-1,-1, 1}, // DOWN
		{1,-1,1, 1},
		{-1,-1,1, 1},
		{-1,-1,-1, 1},

		{-1,-1,-1, 1}, // LEFT
		{-1,-1,1, 1},
		{-1,1,1, 1},
		{-1,1,-1, 1},

		{1,1,1, 1}, // UP
		{1,1,-1, 1},
		{-1,1,-1, 1},
		{-1,1,1, 1},
	};

	float normals[24][3] = {
	{ 0, 0, -1 }, // BACK
	{ 0, 0, -1 },
	{ 0, 0, -1 },
	{ 0, 0, -1 },

	{ 0, 0, 1 }, // FRONT
	{ 0, 0, 1 },
	{ 0, 0, 1 },
	{ 0, 0, 1 },

	{ 1, 0, 0 }, // RIGHT
	{ 1, 0, 0 },
	{ 1, 0, 0 },
	{ 1, 0, 0 },

	{ 0, -1, 0 }, // DOWN
	{ 0, -1, 0 },
	{ 0, -1, 0 },
	{ 0, -1, 0 },

	{ -1, 0, 0 }, // LEFT
	{ -1, 0, 0 },
	{ -1, 0, 0 },
	{ -1, 0, 0 },

	{ 0, 1, 0 }, // UP
	{ 0, 1, 0 },
	{ 0, 1, 0 },
	{ 0, 1, 0 }
	};
	float tangents[24][3] = {
	{ 1, 0, 0 }, // BACK
	{ 1, 0, 0 },
	{ 1, 0, 0 },
	{ 1, 0, 0 },

	{ -1, 0, 0 }, // FRONT
	{ -1, 0, 0 },
	{ -1, 0, 0 },
	{ -1, 0, 0 },

	{ 0, 0, 1 }, // RIGHT
	{ 0, 0, 1 },
	{ 0, 0, 1 },
	{ 0, 0, 1 },

	{ 1, 0, 0 }, // DOWN
	{ 1, 0, 0 },
	{ 1, 0, 0 },
	{ 1, 0, 0 },

	{ 0, 0, -1 }, // LEFT
	{ 0, 0, -1 },
	{ 0, 0, -1 },
	{ 0, 0, -1 },

	{ 1, 0, 0 }, // UP
	{ 1, 0, 0 },
	{ 1, 0, 0 },
	{ 1, 0, 0 }
	};

	std::vector<Vertex> vertexList;
	vertexList.reserve(24);

	vertexList.emplace_back(pos[0], emptyColor, uv[0], normals[0], tangents[0]);
	vertexList.emplace_back(pos[1], emptyColor, uv[1], normals[1], tangents[1]);
	vertexList.emplace_back(pos[2], emptyColor, uv[2], normals[2], tangents[2]);
	vertexList.emplace_back(pos[3], emptyColor, uv[3], normals[3], tangents[3]);
	vertexList.emplace_back(pos[4], emptyColor, uv[4], normals[4], tangents[4]);
	vertexList.emplace_back(pos[5], emptyColor, uv[5], normals[5], tangents[5]);
	vertexList.emplace_back(pos[6], emptyColor, uv[6], normals[6], tangents[6]);
	vertexList.emplace_back(pos[7], emptyColor, uv[7], normals[7], tangents[7]);
	vertexList.emplace_back(pos[8], emptyColor, uv[8], normals[8], tangents[8]);
	vertexList.emplace_back(pos[9], emptyColor, uv[9], normals[9], tangents[9]);
	vertexList.emplace_back(pos[10], emptyColor, uv[10], normals[10], tangents[10]);
	vertexList.emplace_back(pos[11], emptyColor, uv[11], normals[11], tangents[11]);
	vertexList.emplace_back(pos[12], emptyColor, uv[12], normals[12], tangents[12]);
	vertexList.emplace_back(pos[13], emptyColor, uv[13], normals[13], tangents[13]);
	vertexList.emplace_back(pos[14], emptyColor, uv[14], normals[14], tangents[14]);
	vertexList.emplace_back(pos[15], emptyColor, uv[15], normals[15], tangents[15]);
	vertexList.emplace_back(pos[16], emptyColor, uv[16], normals[16], tangents[16]);
	vertexList.emplace_back(pos[17], emptyColor, uv[17], normals[17], tangents[17]);
	vertexList.emplace_back(pos[18], emptyColor, uv[18], normals[18], tangents[18]);
	vertexList.emplace_back(pos[19], emptyColor, uv[19], normals[19], tangents[19]);
	vertexList.emplace_back(pos[20], emptyColor, uv[20], normals[20], tangents[20]);
	vertexList.emplace_back(pos[21], emptyColor, uv[21], normals[21], tangents[21]);
	vertexList.emplace_back(pos[22], emptyColor, uv[22], normals[22], tangents[22]);
	vertexList.emplace_back(pos[23], emptyColor, uv[23], normals[23], tangents[23]);

	std::vector<unsigned> indexList = {
		0,1,2,
		0,2,3,
		4,5,6,
		4,6,7,
		8,9,10,
		8,10,11,
		12,13,14,
		12,14,15,
		16,17,18,
		16,18,19,
		20,21,22,
		20,22,23
	};

	Mesh cube;
	Mesh::Submesh submesh;
	submesh.MaterialIndex = 0;
	submesh.AddLOD(0, std::move(vertexList), std::move(indexList));
	cube.AddSubmesh(std::move(submesh));
	cube.InitBoundingBox({ -1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 1.0f });
	return cube;
}

Mesh ResourceVendor::CreateRampPrimitive() const
{
	float emptyColor[4][4] = { {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f} };
	float uv[18][4][2] = { {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

						   {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

						   {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

						   {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

						   {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
	};

	float pos[18][4] = {
		{-1, 1, 1, 1}, // LEFT
		{-1, 1, -1, 1},
		{-1, -1, -1, 1},
		{-1, -1, 1, 1},

		{1, -1, -1, 1}, // DOWN
		{1, -1, 1, 1},
		{-1, -1, 1, 1},
		{-1, -1, -1, 1},

		{-1, 1, 1, 1}, // DIAGONAL
		{1, -1, 1, 1},
		{1, -1, -1, 1},
		{-1, 1, -1, 1},

		{-1, 1, 1, 1}, // FRONT
		{-1, -1, 1, 1},
		{1, -1, 1, 1},

		{1, -1, -1, 1}, // BACK
		{-1, -1, -1, 1},
		{ -1, 1, -1, 1 },
	};

	float normals[18][3] = {
		{ -1, 0, 0 }, // LEFT
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },

		{ 0, -1, 0 }, // DOWN
		{ 0, -1, 0 },
		{ 0, -1, 0 },
		{ 0, -1, 0 },

		{ 1, 1, 0 }, // DIAGONAL
		{ 1, 1, 0 },
		{ 1, 1, 0 },
		{ 1, 1, 0 },

		{ 0, 0, 1 }, // FRONT
		{ 0, 0, 1 },
		{ 0, 0, 1 },

		{ 0, 0, -1 }, // BACK
		{ 0, 0, -1 },
		{ 0, 0, -1 }
	};

	float tangents[18][3] = {
		{ 0, 0, -1 }, // LEFT
		{ 0, 0, -1 },
		{ 0, 0, -1 },
		{ 0, 0, -1 },

		{ 0, 0, 1 }, // DOWN
		{ 0, 0, 1 },
		{ 0, 0, 1 },
		{ 0, 0, 1 },

		{ 0, 0, 1 }, // DIAGONAL
		{ 0, 0, 1 },
		{ 0, 0, 1 },
		{ 0, 0, 1 },

		{ -1, 0, 0 }, // FRONT
		{ -1, 0, 0 },
		{ -1, 0, 0 },

		{ 1, 0, 0 }, // BACK
		{ 1, 0, 0 },
		{ 1, 0, 0 }
	};

	std::vector<Vertex> vertexList;
	vertexList.reserve(18);

	vertexList.emplace_back(pos[0], emptyColor, uv[0], normals[0], tangents[0]);
	vertexList.emplace_back(pos[1], emptyColor, uv[1], normals[1], tangents[1]);
	vertexList.emplace_back(pos[2], emptyColor, uv[2], normals[2], tangents[2]);
	vertexList.emplace_back(pos[3], emptyColor, uv[3], normals[3], tangents[3]);
	vertexList.emplace_back(pos[4], emptyColor, uv[4], normals[4], tangents[4]);
	vertexList.emplace_back(pos[5], emptyColor, uv[5], normals[5], tangents[5]);
	vertexList.emplace_back(pos[6], emptyColor, uv[6], normals[6], tangents[6]);
	vertexList.emplace_back(pos[7], emptyColor, uv[7], normals[7], tangents[7]);
	vertexList.emplace_back(pos[8], emptyColor, uv[8], normals[8], tangents[8]);
	vertexList.emplace_back(pos[9], emptyColor, uv[9], normals[9], tangents[9]);
	vertexList.emplace_back(pos[10], emptyColor, uv[10], normals[10], tangents[10]);
	vertexList.emplace_back(pos[11], emptyColor, uv[11], normals[11], tangents[11]);
	vertexList.emplace_back(pos[12], emptyColor, uv[12], normals[12], tangents[12]);
	vertexList.emplace_back(pos[13], emptyColor, uv[13], normals[13], tangents[13]);
	vertexList.emplace_back(pos[14], emptyColor, uv[14], normals[14], tangents[14]);
	vertexList.emplace_back(pos[15], emptyColor, uv[15], normals[15], tangents[15]);
	vertexList.emplace_back(pos[16], emptyColor, uv[16], normals[16], tangents[16]);
	vertexList.emplace_back(pos[17], emptyColor, uv[17], normals[17], tangents[17]);

	std::vector<unsigned> indexList = {
		0, 1, 2,
		2, 3, 0,

		4, 5, 6,
		6, 7, 4,

		8, 9, 10,
		10, 11, 8,

		12, 13, 14,
		15, 16, 17,
	};

	Mesh ramp;
	Mesh::Submesh submesh;
	submesh.MaterialIndex = 0;
	submesh.AddLOD(0, std::move(vertexList), std::move(indexList));
	ramp.AddSubmesh(std::move(submesh));
	ramp.InitBoundingBox({ -1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 1.0f });
	return ramp;
}