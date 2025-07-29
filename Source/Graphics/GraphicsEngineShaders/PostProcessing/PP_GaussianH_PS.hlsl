#include "../Includes/DefaultShaderIncludes.hlsli"

Texture2D InputTexture : register(t30);

float4 main(Quad_VS_to_PS input) : SV_TARGET
{
    float texelSize = 1.0f / FB_Resolution.x;
    uint kernelSize = 5;
    float step = (((float) kernelSize - 1.0f) / 2.0f) * -1.0f;
    float3 result = 0;
    for (uint s = 0; s < kernelSize; ++s)
    {
        float2 uv = input.UV + float2(texelSize * (step + (float) s), 0.0f);
        float3 color = InputTexture.Sample(AnisoWrapSampler, uv).rgb;
        result += color * GaussianKernel[s];
    }
    
    return float4(result, 1.0f);
}