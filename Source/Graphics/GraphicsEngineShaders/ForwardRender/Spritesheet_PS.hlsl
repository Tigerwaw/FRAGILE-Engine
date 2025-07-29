#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/Sprite_Includes.hlsli"
#include "../Includes/ConstantBuffers/SpriteBuffer.hlsli"
#include "../Includes/ConstantBuffers/FrameBuffer.hlsli"

float4 main(Sprite_GSout input) : SV_TARGET
{
    float2 uv = Flipbook(input.TexCoord0.xy, SPB_SpritesheetDimensions, SPB_CurrentFrame);
    float4 color = AlbedoTexture.Sample(AnisoWrapSampler, uv);
	return color;
}