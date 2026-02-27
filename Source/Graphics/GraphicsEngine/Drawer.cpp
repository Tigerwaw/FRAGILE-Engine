#include "GraphicsEngine.pch.h"
#include "Drawer.h"

#include "GraphicsSettings.hpp"
#include "Objects/ParticleSystem/ParticleEmitter.h"
#include "Objects/ParticleSystem/TrailEmitter.h"
#include "Objects/Mesh.h"
#include "Objects/Material.h"
#include "Objects/DynamicVertexBuffer.h"
#include "Objects/Text/Text.h"
#include "Objects/PipelineStateObject.h"

#include "Objects/ConstantBuffers/MaterialBuffer.h"

void Drawer::RenderQuad()
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GE Render Quad");
	GraphicsEngine& ge = GraphicsEngine::Get();
	ge.SetTextureResource_PS(127, *ge.myBRDFLUTTexture);

	ge.myRHI->SetPrimitiveTopology(Topology::TRIANGLESTRIP);
	ge.myRHI->SetVertexBuffer(nullptr, 0, 0);
	ge.myRHI->SetIndexBuffer(nullptr);
	ge.myRHI->SetInputLayout(nullptr);
	ge.myRHI->Draw(4);
	ge.ClearTextureResource_PS(127);
}

void Drawer::RenderSkybox(const Mesh& aMesh, const std::shared_ptr<Texture> aTexture)
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GE Render Skybox");
	GraphicsEngine& ge = GraphicsEngine::Get();
	ge.ChangePipelineState(ge.GetPSO(PSOType::Skybox));
	ge.myRHI->SetPrimitiveTopology(Topology::TRIANGLELIST);

	for (const auto& submesh : aMesh.GetSubmeshes())
	{
		const Mesh::Submesh::LOD& lod = submesh.GetLOD(0);
		ge.myRHI->SetVertexBuffer(lod.GetVertexBuffer(), ge.myCurrentPSO->VertexStride, 0);
		ge.myRHI->SetIndexBuffer(lod.GetIndexBuffer());

		ge.SetTextureResource_PS(0, *aTexture);
		ge.myRHI->DrawIndexed(0, lod.GetNumIndices());
		ge.myDrawcallAmount++;

		ge.ClearTextureResource_PS(0);
	}
}

void Drawer::RenderMesh(const Mesh& aMesh, const std::vector<std::shared_ptr<Material>>& aMaterialList, float aLODHeuristic)
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GE Render Mesh");
	GraphicsEngine& ge = GraphicsEngine::Get();
	ge.myRHI->SetPrimitiveTopology(Topology::TRIANGLELIST);

	ge.SetTextureResource_PS(127, *ge.myBRDFLUTTexture);

	for (const auto& submesh : aMesh.GetSubmeshes())
	{
		const Mesh::Submesh::LOD& lod = submesh.GetLOD(submesh.GetAppropriateLODLevel(aLODHeuristic));
		ge.myRHI->SetVertexBuffer(lod.GetVertexBuffer(), ge.myCurrentPSO->VertexStride, 0);
		ge.myRHI->SetIndexBuffer(lod.GetIndexBuffer());
		std::array<unsigned, GraphicsSettings::MAX_MATERIAL_TEXTURES> slotsToClear = { 0 };
		unsigned currentSlotIndex = 0;

		if (aMaterialList.size() > submesh.MaterialIndex)
		{
			MaterialBuffer matBufferData = aMaterialList[submesh.MaterialIndex]->MaterialSettings();
			ge.UpdateAndSetConstantBuffer(ConstantBufferType::MaterialBuffer, matBufferData);

			ge.ChangePipelineState(aMaterialList[submesh.MaterialIndex]->GetPSO());

			for (auto& [slot, texture] : aMaterialList[submesh.MaterialIndex]->GetTextures())
			{
				ge.SetTextureResource_PS(slot, *texture);
				slotsToClear[currentSlotIndex] = slot;
				++currentSlotIndex;
			}
		}
		else
		{
			auto defaultMat = GraphicsEngine::Get().GetDefaultMaterial();
			ge.UpdateAndSetConstantBuffer(ConstantBufferType::MaterialBuffer, defaultMat->MaterialSettings());

			ge.ChangePipelineState(defaultMat->GetPSO());

			for (auto& [slot, texture] : defaultMat->GetTextures())
			{
				ge.SetTextureResource_PS(slot, *texture);
				slotsToClear[currentSlotIndex] = slot;
				++currentSlotIndex;
			}
		}

		ge.myRHI->DrawIndexed(0, lod.GetNumIndices());
		ge.myDrawcallAmount++;

		for (unsigned slotIndex = 0; slotIndex < currentSlotIndex; slotIndex++)
		{
			ge.ClearTextureResource_PS(slotsToClear[slotIndex]);
		}
	}

	ge.ClearTextureResource_PS(127);
}

void Drawer::RenderMeshShadow(const Mesh& aMesh)
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GE Render Mesh Shadow");
	GraphicsEngine& ge = GraphicsEngine::Get();
	ge.myRHI->SetPrimitiveTopology(Topology::TRIANGLELIST);

	for (const auto& submesh : aMesh.GetSubmeshes())
	{
		const Mesh::Submesh::LOD& lod = submesh.GetLOD(0);
		ge.myRHI->SetVertexBuffer(lod.GetVertexBuffer(), ge.myCurrentPSO->VertexStride, 0);
		ge.myRHI->SetIndexBuffer(lod.GetIndexBuffer());
		ge.myRHI->DrawIndexed(0, lod.GetNumIndices());
		ge.myDrawcallAmount++;
	}
}

void Drawer::RenderMeshDebugPass(const Mesh& aMesh, const std::vector<std::shared_ptr<Material>>& aMaterialList, float aLODHeuristic)
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GE Render Mesh");
	GraphicsEngine& ge = GraphicsEngine::Get();
	ge.myRHI->SetPrimitiveTopology(Topology::TRIANGLELIST);

	ge.SetTextureResource_PS(127, *ge.myBRDFLUTTexture);

	for (const auto& submesh : aMesh.GetSubmeshes())
	{
		const Mesh::Submesh::LOD& lod = submesh.GetLOD(submesh.GetAppropriateLODLevel(aLODHeuristic));
		ge.myRHI->SetVertexBuffer(lod.GetVertexBuffer(), ge.myCurrentPSO->VertexStride, 0);
		ge.myRHI->SetIndexBuffer(lod.GetIndexBuffer());

		std::array<unsigned, GraphicsSettings::MAX_MATERIAL_TEXTURES> slotsToClear = { 0 };
		unsigned currentSlotIndex = 0;

		if (aMaterialList.size() > submesh.MaterialIndex)
		{
			MaterialBuffer matBufferData = aMaterialList[submesh.MaterialIndex]->MaterialSettings();
			ge.UpdateAndSetConstantBuffer(ConstantBufferType::MaterialBuffer, matBufferData);

			for (auto& [slot, texture] : aMaterialList[submesh.MaterialIndex]->GetTextures())
			{
				ge.SetTextureResource_PS(slot, *texture);
				slotsToClear[currentSlotIndex] = slot;
				++currentSlotIndex;
			}
		}
		else
		{
			auto defaultMat = GraphicsEngine::Get().GetDefaultMaterial();
			ge.UpdateAndSetConstantBuffer(ConstantBufferType::MaterialBuffer, defaultMat->MaterialSettings());

			for (auto& [slot, texture] : defaultMat->GetTextures())
			{
				ge.SetTextureResource_PS(slot, *texture);
				slotsToClear[currentSlotIndex] = slot;
				++currentSlotIndex;
			}
		}

		ge.myRHI->DrawIndexed(0, lod.GetNumIndices());
		ge.myDrawcallAmount++;

		for (unsigned slotIndex = 0; slotIndex < currentSlotIndex; slotIndex++)
		{
			ge.ClearTextureResource_PS(slotsToClear[slotIndex]);
		}
	}

	ge.ClearTextureResource_PS(127);
}

void Drawer::RenderInstancedMesh(const Mesh& aMesh, unsigned aMeshCount, const std::vector<std::shared_ptr<Material>>& aMaterialList, DynamicVertexBuffer& aInstanceBuffer)
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GE Render Instanced Mesh");
	GraphicsEngine& ge = GraphicsEngine::Get();

	ge.myRHI->SetPrimitiveTopology(Topology::TRIANGLELIST);

	ge.SetTextureResource_PS(127, *ge.myBRDFLUTTexture);

	for (const auto& submesh : aMesh.GetSubmeshes())
	{
		std::vector<ID3D11Buffer*> buffers;
		std::vector<unsigned> strides;
		std::vector<unsigned> offsets;

		const Mesh::Submesh::LOD& lod = submesh.GetLOD(0);
		buffers.emplace_back(*lod.GetVertexBuffer().GetAddressOf());
		buffers.emplace_back(*aInstanceBuffer.GetVertexBuffer().GetAddressOf());

		strides.emplace_back(ge.myCurrentPSO->VertexStride);
		strides.emplace_back(static_cast<unsigned>(sizeof(Math::Matrix4x4f)));

		offsets.emplace_back(0);
		offsets.emplace_back(0);

		ge.myRHI->SetVertexBuffers(buffers, strides, offsets);
		ge.myRHI->SetIndexBuffer(lod.GetIndexBuffer());

		std::array<unsigned, GraphicsSettings::MAX_MATERIAL_TEXTURES> slotsToClear = { 0 };
		unsigned currentSlotIndex = 0;

		if (aMaterialList.size() > submesh.MaterialIndex)
		{
			MaterialBuffer matBufferData = aMaterialList[submesh.MaterialIndex]->MaterialSettings();
			ge.UpdateAndSetConstantBuffer(ConstantBufferType::MaterialBuffer, matBufferData);

			ge.ChangePipelineState(aMaterialList[submesh.MaterialIndex]->GetPSO());

			for (auto& [slot, texture] : aMaterialList[submesh.MaterialIndex]->GetTextures())
			{
				ge.SetTextureResource_PS(slot, *texture);
				slotsToClear[currentSlotIndex] = slot;
				++currentSlotIndex;
			}
		}
		else
		{
			auto defaultMat = GraphicsEngine::Get().GetDefaultMaterial();
			ge.UpdateAndSetConstantBuffer(ConstantBufferType::MaterialBuffer, defaultMat->MaterialSettings());

			ge.ChangePipelineState(defaultMat->GetPSO());

			for (auto& [slot, texture] : defaultMat->GetTextures())
			{
				ge.SetTextureResource_PS(slot, *texture);
				slotsToClear[currentSlotIndex] = slot;
				++currentSlotIndex;
			}
		}

		ge.myRHI->DrawIndexedInstanced(lod.GetNumIndices(), aMeshCount, 0, 0, 0);
		ge.myDrawcallAmount++;

		for (unsigned slotIndex = 0; slotIndex < currentSlotIndex; slotIndex++)
		{
			ge.ClearTextureResource_PS(slotsToClear[slotIndex]);
		}
	}

	ge.ClearTextureResource_PS(127);
}

void Drawer::RenderInstancedMeshShadow(const Mesh& aMesh, unsigned aMeshCount, DynamicVertexBuffer& aInstanceBuffer)
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GE Render Instanced Mesh Shadow");
	GraphicsEngine& ge = GraphicsEngine::Get();

	ge.myRHI->SetPrimitiveTopology(Topology::TRIANGLELIST);

	for (const auto& submesh : aMesh.GetSubmeshes())
	{
		std::vector<ID3D11Buffer*> buffers;
		std::vector<unsigned> strides;
		std::vector<unsigned> offsets;

		const Mesh::Submesh::LOD& lod = submesh.GetLOD(0);
		buffers.emplace_back(*lod.GetVertexBuffer().GetAddressOf());
		buffers.emplace_back(*aInstanceBuffer.GetVertexBuffer().GetAddressOf());

		strides.emplace_back(ge.myCurrentPSO->VertexStride);
		strides.emplace_back(static_cast<unsigned>(sizeof(Math::Matrix4x4f)));

		offsets.emplace_back(0);
		offsets.emplace_back(0);

		ge.myRHI->SetVertexBuffers(buffers, strides, offsets);
		ge.myRHI->SetIndexBuffer(lod.GetIndexBuffer());

		ge.myRHI->DrawIndexedInstanced(lod.GetNumIndices(), aMeshCount, 0, 0, 0);
		ge.myDrawcallAmount++;
	}
}

void Drawer::RenderInstancedMeshDebugPass(const Mesh& aMesh, unsigned aMeshCount, const std::vector<std::shared_ptr<Material>>& aMaterialList, DynamicVertexBuffer& aInstanceBuffer)
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GE Render Instanced Mesh Debug Pass");
	GraphicsEngine& ge = GraphicsEngine::Get();

	ge.myRHI->SetPrimitiveTopology(Topology::TRIANGLELIST);

	ge.SetTextureResource_PS(127, *ge.myBRDFLUTTexture);

	for (const auto& submesh : aMesh.GetSubmeshes())
	{
		std::vector<ID3D11Buffer*> buffers;
		std::vector<unsigned> strides;
		std::vector<unsigned> offsets;

		const Mesh::Submesh::LOD& lod = submesh.GetLOD(0);
		buffers.emplace_back(*lod.GetVertexBuffer().GetAddressOf());
		buffers.emplace_back(*aInstanceBuffer.GetVertexBuffer().GetAddressOf());

		strides.emplace_back(ge.myCurrentPSO->VertexStride);
		strides.emplace_back(static_cast<unsigned>(sizeof(Math::Matrix4x4f)));

		offsets.emplace_back(0);
		offsets.emplace_back(0);

		ge.myRHI->SetVertexBuffers(buffers, strides, offsets);
		ge.myRHI->SetIndexBuffer(lod.GetIndexBuffer());

		std::array<unsigned, GraphicsSettings::MAX_MATERIAL_TEXTURES> slotsToClear = { 0 };
		unsigned currentSlotIndex = 0;

		if (aMaterialList.size() > submesh.MaterialIndex)
		{
			MaterialBuffer matBufferData = aMaterialList[submesh.MaterialIndex]->MaterialSettings();
			ge.UpdateAndSetConstantBuffer(ConstantBufferType::MaterialBuffer, matBufferData);

			for (auto& [slot, texture] : aMaterialList[submesh.MaterialIndex]->GetTextures())
			{
				ge.SetTextureResource_PS(slot, *texture);
				slotsToClear[currentSlotIndex] = slot;
				++currentSlotIndex;
			}
		}
		else
		{
			auto defaultMat = GraphicsEngine::Get().GetDefaultMaterial();
			ge.UpdateAndSetConstantBuffer(ConstantBufferType::MaterialBuffer, defaultMat->MaterialSettings());

			for (auto& [slot, texture] : defaultMat->GetTextures())
			{
				ge.SetTextureResource_PS(slot, *texture);
				slotsToClear[currentSlotIndex] = slot;
				++currentSlotIndex;
			}
		}

		ge.myRHI->DrawIndexedInstanced(lod.GetNumIndices(), aMeshCount, 0, 0, 0);
		ge.myDrawcallAmount++;

		for (unsigned slotIndex = 0; slotIndex < currentSlotIndex; slotIndex++)
		{
			ge.ClearTextureResource_PS(slotsToClear[slotIndex]);
		}
	}

	ge.ClearTextureResource_PS(127);
}

void Drawer::RenderSprite()
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GE Render Sprite");
	GraphicsEngine& ge = GraphicsEngine::Get();

	ge.myRHI->SetPrimitiveTopology(Topology::POINTLIST);
	ge.myRHI->Draw(1);
	ge.myDrawcallAmount++;
}

void Drawer::RenderText(const Text& aText)
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GE Render Text");
	GraphicsEngine& ge = GraphicsEngine::Get();

	ge.SetTextureResource_PS(0, *aText.GetTexture());

	const Text::TextData& textData = aText.GetTextData();
	ge.myRHI->SetVertexBuffer(textData.vertexBuffer->GetVertexBuffer(), ge.myCurrentPSO->VertexStride, 0);
	ge.myRHI->SetIndexBuffer(textData.indexBuffer);
	ge.myRHI->SetPrimitiveTopology(Topology::TRIANGLELIST);

	ge.myRHI->DrawIndexed(0, textData.numIndices);
	ge.myDrawcallAmount++;

	ge.ClearTextureResource_PS(0);
}

void Drawer::RenderDebugLines(const DynamicVertexBuffer& aDynamicBuffer, unsigned aLineAmount)
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GE Render Debug Lines");
	GraphicsEngine& ge = GraphicsEngine::Get();

	ge.myRHI->SetVertexBuffer(aDynamicBuffer.GetVertexBuffer(), ge.myCurrentPSO->VertexStride, 0);
	ge.myRHI->SetPrimitiveTopology(Topology::POINTLIST);
	ge.myRHI->Draw(aLineAmount);
}

void Drawer::RenderParticleEmitter(const ParticleEmitter& aParticleEmitter)
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GE Render Particle Emitter");
	GraphicsEngine& ge = GraphicsEngine::Get();

	std::array<unsigned, GraphicsSettings::MAX_MATERIAL_TEXTURES> slotsToClear = { 0 };
	unsigned currentSlotIndex = 0;

	if (aParticleEmitter.GetMaterial())
	{
		Material& mat = *aParticleEmitter.GetMaterial();
		ge.UpdateAndSetConstantBuffer(ConstantBufferType::MaterialBuffer, mat.MaterialSettings());

		ge.ChangePipelineState(mat.GetPSO());

		for (auto& [slot, texture] : mat.GetTextures())
		{
			ge.SetTextureResource_PS(slot, *texture);
			slotsToClear[currentSlotIndex] = slot;
			++currentSlotIndex;
		}
	}
	else
	{
		Material& defaultMat = *GraphicsEngine::Get().GetDefaultMaterial();
		ge.UpdateAndSetConstantBuffer(ConstantBufferType::MaterialBuffer, defaultMat.MaterialSettings());

		ge.ChangePipelineState(ge.GetPSO(PSOType::Particle));

		for (auto& [slot, texture] : defaultMat.GetTextures())
		{
			ge.SetTextureResource_PS(slot, *texture);
			slotsToClear[currentSlotIndex] = slot;
			++currentSlotIndex;
		}
	}

	ge.myRHI->SetPrimitiveTopology(Topology::POINTLIST);
	ge.myRHI->SetVertexBuffer(aParticleEmitter.myVertexBuffer->GetVertexBuffer(), ge.myCurrentPSO->VertexStride, 0);
	ge.myRHI->Draw(static_cast<unsigned>(aParticleEmitter.myParticles.size()));
	ge.myDrawcallAmount++;

	for (unsigned slotIndex = 0; slotIndex < currentSlotIndex; slotIndex++)
	{
		ge.ClearTextureResource_PS(slotsToClear[slotIndex]);
	}
}

void Drawer::RenderTrailEmitter(const TrailEmitter& aTrailEmitter)
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GE Render Trail Emitter");
	GraphicsEngine& ge = GraphicsEngine::Get();

	std::array<unsigned, GraphicsSettings::MAX_MATERIAL_TEXTURES> slotsToClear = { 0 };
	unsigned currentSlotIndex = 0;

	if (aTrailEmitter.GetMaterial())
	{
		Material& mat = *aTrailEmitter.GetMaterial();
		ge.UpdateAndSetConstantBuffer(ConstantBufferType::MaterialBuffer, mat.MaterialSettings());

		ge.ChangePipelineState(mat.GetPSO());

		for (auto& [slot, texture] : mat.GetTextures())
		{
			ge.SetTextureResource_PS(slot, *texture);
			slotsToClear[currentSlotIndex] = slot;
			++currentSlotIndex;
		}
	}
	else
	{
		Material& defaultMat = *GraphicsEngine::Get().GetDefaultMaterial();
		ge.UpdateAndSetConstantBuffer(ConstantBufferType::MaterialBuffer, defaultMat.MaterialSettings());

		ge.ChangePipelineState(ge.GetPSO(PSOType::Trail));

		for (auto& [slot, texture] : defaultMat.GetTextures())
		{
			ge.SetTextureResource_PS(slot, *texture);
			slotsToClear[currentSlotIndex] = slot;
			++currentSlotIndex;
		}
	}

	ge.myRHI->SetPrimitiveTopology(Topology::LINESTRIP);
	ge.myRHI->SetVertexBuffer(aTrailEmitter.myVertexBuffer->GetVertexBuffer(), ge.myCurrentPSO->VertexStride, 0);
	unsigned count = aTrailEmitter.GetCurrentLength();
	ge.myRHI->Draw(count);
	ge.myDrawcallAmount++;

	for (unsigned slotIndex = 0; slotIndex < currentSlotIndex; slotIndex++)
	{
		ge.ClearTextureResource_PS(slotsToClear[slotIndex]);
	}
}