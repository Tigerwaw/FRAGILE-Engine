#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/Sprite_Includes.hlsli"
Texture2D FontTexture : register(t30);

float median(float3 rgb)
{
    return max(min(rgb.r, rgb.g), min(max(rgb.r, rgb.g), rgb.b));
}

float2 SafeNormalize(float2 v)
{
    float len = length(v);
    len = (len > 0.0) ? 1.0 / len : 0.0;
    return v * len;
}

float ScreenPxRange(float2 uvs)
{
    const float screenPxRange = 2; // Default for MSDF-atlas-gen
    const float2 atlasSize = float2(388, 388);
    float2 unitRange = float2(screenPxRange, screenPxRange) / atlasSize;
    float2 screenTexSize = float2(1.0, 1.0) / fwidth(uvs);
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

float4 main(Text_VSout input) : SV_TARGET
{    
    const float3 fontPx = FontTexture.Sample(AnisoWrapSampler, input.TexCoord).rgb;
    float sd = median(fontPx);
    float screenPxDist = ScreenPxRange(input.TexCoord) * (sd - 0.5);
    float opacity = clamp(screenPxDist + 0.5, 0, 1.0);
    float3 bgColor = float3(0, 0, 0);
    float3 fgColor = float3(1, 1, 1);
    
    float4 result;
    result.rgb = lerp(bgColor, fgColor, opacity);
    result.a = opacity;
    if (result.a < 0.05f)
    {
        discard;
        return 0;
    }
    return result;
}