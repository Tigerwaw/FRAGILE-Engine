#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/PostProcessing_Includes.hlsli"
#include "../Includes/ConstantBuffers/PostProcessBuffer.hlsli"

Texture2D IntermediateLDRBuffer : register(t30);

float4 main(Quad_VS_to_PS input) : SV_TARGET
{
    const float4 color = IntermediateLDRBuffer.Sample(AnisoWrapSampler, input.UV);
    
    if (color.a < 0.05f)
    {
        discard;
        return 0;
    }
    
    switch (PPB_LuminanceFunction)
    {
        case 0:
            return LuminanceRandomGain(color.rgb, 5);
            break;
        case 1:
            return LuminanceContrast(color.rgb, 0.8f);
            break;
        case 2:
            return LuminanceReductionAndGain(color.rgb, 0.8f);
            break;
        case 3:
            return LuminanceFade(color.rgb, 0.8f, 5);
            break;
        default:
            return LuminanceFade(color.rgb, 0.8f, 5);
            break;
    }
}