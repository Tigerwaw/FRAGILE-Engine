#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/Tonemap.hlsli"

Texture2D IntermediateHDRBuffer : register(t30);

float4 main(Quad_VS_to_PS input) : SV_TARGET
{
    return float4(Tonemap_UnrealEngine(IntermediateHDRBuffer.Sample(AnisoWrapSampler, input.UV).rgb), 1.0f);
}