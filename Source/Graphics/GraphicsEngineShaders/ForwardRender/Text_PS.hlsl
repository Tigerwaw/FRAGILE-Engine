#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/Sprite_Includes.hlsli"

Texture2D FontTexture : register(t0);

float median(float3 rgb)
{
    return max(min(rgb.r, rgb.g), min(max(rgb.r, rgb.g), rgb.b));
}

float ScreenPxRange(float2 uvs, float pixelRange, float2 textureSize)
{
    float2 unitRange = float2(pixelRange, pixelRange) / textureSize;
    float2 screenTexSize = float2(1.0, 1.0) / fwidth(uvs);
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

float4 main(Text_VSout input) : SV_TARGET
{    
    float4 msd = FontTexture.SampleLevel(AnisoWrapSampler, input.TexCoord, 0);
    float sd = median(msd.rgb);
    
    float2 dim;
    float maps;
    FontTexture.GetDimensions(0, dim.x, dim.y, maps);
    float screenPxDist = ScreenPxRange(input.TexCoord, 2.0f, dim) * (sd - 0.5);
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