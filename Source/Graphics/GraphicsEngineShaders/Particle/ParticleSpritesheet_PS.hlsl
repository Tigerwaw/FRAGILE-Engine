#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/ParticleIncludes.hlsli"
#include "../Includes/Sprite_Includes.hlsli"
#include "../Includes/ConstantBuffers/SpriteBuffer.hlsli"
#include "../Includes/ConstantBuffers/FrameBuffer.hlsli"

Texture2D ParticleTexture : register(t0);

float4 main(Particle_GSOut input) : SV_TARGET
{	
    if (input.Lifetime < 0.0f)
    {
        discard;
        return 0;
    }
    
    float2 uv = Flipbook(input.UV, float2(8, 4), input.Lifetime * (8 * 4));
    const float4 textureColor = ParticleTexture.Sample(AnisoWrapSampler, uv);
    return textureColor * input.Color;
}