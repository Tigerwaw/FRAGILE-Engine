#include "GraphicsEngine.pch.h"
#include "UpdatePostProcessBuffer.h"

UpdatePostProcessBuffer::UpdatePostProcessBuffer()
{
}

void UpdatePostProcessBuffer::Execute()
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD UpdatePostProcessBuffer Execute");
	PostProcessingSettings& ppSettings = GraphicsEngine::Get().GetPostProcessingSettings();

	PostProcessBuffer ppBufferData;
	const std::vector<Math::Vector4f>& randomKernel = ppSettings.GetRandomKernel();
	unsigned kernelSize = static_cast<unsigned>(randomKernel.size());
	unsigned dataSize = sizeof(Math::Vector4f) * kernelSize;
	
	memcpy_s(ppBufferData.RandomKernel, dataSize, randomKernel.data(), dataSize);

	ppBufferData.KernelSize = kernelSize;
	ppBufferData.SSAOEnabled = static_cast<unsigned>(ppSettings.SSAOEnabled);
	ppBufferData.BloomFunction = static_cast<unsigned>(ppSettings.BloomFunction);
	ppBufferData.LuminanceFunction = static_cast<unsigned>(ppSettings.LuminanceFunction);
	ppBufferData.SSAONoisePower = ppSettings.SSAONoisePower;
	ppBufferData.SSAORadius = ppSettings.SSAORadius;
	ppBufferData.SSAOBias = ppSettings.SSAOBias;
	ppBufferData.BloomStrength = ppSettings.BloomStrength;

	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::PostProcessBuffer, ppBufferData);
}