#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/PostProcessing_Includes.hlsli"
#include "../Includes/ConstantBuffers/PostProcessBuffer.hlsli"

Texture2D InputTexture : register(t30);
Texture2D IntermediateLDR : register(t31);

float4 main(Quad_VS_to_PS input) : SV_TARGET
{
    const float3 sceneColor = IntermediateLDR.Sample(AnisoWrapSampler, input.UV).rgb;
    const float3 bloomColor = InputTexture.Sample(AnisoWrapSampler, input.UV).rgb * PPB_BloomStrength;
    
    switch (PPB_BloomFunction)
    {
        case 0:
            return BloomAdditive(sceneColor, bloomColor);
            break;
        case 1:
            return BloomScaledToScene(sceneColor, bloomColor);
            break;
        case 2:
            return BloomScaledToLuminance(sceneColor, bloomColor);
            break;
        default:
            return BloomScaledToScene(sceneColor, bloomColor);
            break;
    }
}