#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/ConstantBuffers/FrameBuffer.hlsli"
#include "../Includes/ConstantBuffers/LightBuffer.hlsli"
#include "../Includes/ConstantBuffers/MaterialBuffer.hlsli"
#include "../Includes/ConstantBuffers/ObjectBuffer.hlsli"

float4 main(MeshVStoPS input) : SV_TARGET
{
	float2 uv = Flipbook(input.TexCoord0.xy, OB_CustomData_2.zw, ceil(OB_CustomData_2.x * (OB_CustomData_2.z * OB_CustomData_2.w)));
    float4 color = AlbedoTexture.Sample(AnisoWrapSampler, uv);
    
    clip(color.a < 0.01 ? -1 : color.a);
    
    color *= MB_AlbedoTint;
	return color;
}