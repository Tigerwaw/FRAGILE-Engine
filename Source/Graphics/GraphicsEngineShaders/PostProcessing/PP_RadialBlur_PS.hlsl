#include "../Includes/DefaultShaderIncludes.hlsli"

Texture2D InputTexture : register(t30);

float4 main(Quad_VS_to_PS input) : SV_TARGET
{
    uint w, h, m;
    InputTexture.GetDimensions(0, w, h, m);
    const float2 texel = 1.0f / float2(w, h);
    float v = 0;
    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            float2 offset = float2(float(x), float(y)) * texel;
            v += InputTexture.Sample(AnisoWrapSampler, input.UV + offset).r;
        }
    }
    
    return v / (4.0f * 4.0f);
}