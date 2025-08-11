#include "GraphicsEngine.pch.h"
#include "UpdateShadowBuffer.h"
#include "Objects/ConstantBuffers/ShadowBuffer.h"
#include "Math/Quaternion.hpp"

UpdateShadowBuffer::UpdateShadowBuffer(const ShadowData& aShadowData)
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD UpdateShadowBuffer Copy Constructor");
	myData = aShadowData;
}

UpdateShadowBuffer::UpdateShadowBuffer(ShadowData&& aShadowData)
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD UpdateShadowBuffer Move Constructor");
	myData = std::move(aShadowData);
}

void UpdateShadowBuffer::Execute()
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD UpdateShadowBuffer Execute");
	ShadowBuffer shadowBufferData;

	myData.cameraTransform = Math::Matrix4x4f::CreateRollPitchYawMatrix({ 0, 90.0f, 0 }) * myData.cameraTransform;
	shadowBufferData.CameraTransforms[0] = myData.cameraTransform.GetFastInverse();
	
	myData.cameraTransform = Math::Matrix4x4f::CreateRollPitchYawMatrix({ 0, -180.0f, 0 }) * myData.cameraTransform;
	shadowBufferData.CameraTransforms[1] = myData.cameraTransform.GetFastInverse();
	
	myData.cameraTransform = Math::Matrix4x4f::CreateRollPitchYawMatrix({ -90.0f, 90.0f, 0 }) * myData.cameraTransform;
	shadowBufferData.CameraTransforms[2] = myData.cameraTransform.GetFastInverse();
	 
	myData.cameraTransform = Math::Matrix4x4f::CreateRollPitchYawMatrix({ 180.0f, 0, 0 }) * myData.cameraTransform;
	shadowBufferData.CameraTransforms[3] = myData.cameraTransform.GetFastInverse();

	myData.cameraTransform = Math::Matrix4x4f::CreateRollPitchYawMatrix({ -90.0f, 0, 0 }) * myData.cameraTransform;
	shadowBufferData.CameraTransforms[4] = myData.cameraTransform.GetFastInverse();
	
	myData.cameraTransform = Math::Matrix4x4f::CreateRollPitchYawMatrix({ 0, 180.0f, 0 }) * myData.cameraTransform;
	shadowBufferData.CameraTransforms[5] = myData.cameraTransform.GetFastInverse();

	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ShadowBuffer, shadowBufferData);
}