#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/ParticleIncludes.hlsli"

Texture2D ParticleTexture : register(t0);

float4 main(Particle_GSOut input) : SV_TARGET
{
	if (input.Lifetime < 0.0f)
    {
        discard;
        return 0;
    }
    
    float4 textureColor = ParticleTexture.Sample(AnisoWrapSampler, input.UV);
    
    if (input.ChannelMask.r + input.ChannelMask.g + input.ChannelMask.b + input.ChannelMask.a > 1.0)
    {
        return textureColor * input.Color;
    }
    else
    {
        // This is acting on the assumption that a particle sprite is only on a single channel in the texture.
        // It's a little weird but think of the texture being sampled as just being the "shape" of the particle rather than the actual color.
        float4 maskedTexture = textureColor * input.ChannelMask;
        return input.Color * (maskedTexture.r + maskedTexture.g + maskedTexture.b + maskedTexture.a);
    }
}