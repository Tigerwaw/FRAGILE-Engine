#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/ParticleIncludes.hlsli"

Texture2D TrailTexture : register(t0);

float4 main(Trail_GSOut input) : SV_TARGET
{    
    if (input.Lifetime < 0.0f)
    {
        discard;
        return 0;
    }
    
    float4 textureColor = TrailTexture.Sample(AnisoWrapSampler, input.UV);
    
    if (input.ChannelMask.r + input.ChannelMask.g + input.ChannelMask.b + input.ChannelMask.a > 1.0)
    {
        return textureColor * input.Color;
    }
    else
    {
        // This is acting on the assumption that the trail is only on a single channel in the texture.
        // It's a little weird but think of the texture being sampled as just being the "shape" of the trail rather than the actual color.
        float4 maskedTexture = textureColor * input.ChannelMask;
        return input.Color * (maskedTexture.r + maskedTexture.g + maskedTexture.b + maskedTexture.a);
    }
}