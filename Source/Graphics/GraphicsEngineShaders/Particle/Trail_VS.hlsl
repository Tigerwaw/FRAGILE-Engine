#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/ParticleIncludes.hlsli"
#include "../Includes/ConstantBuffers/ObjectBuffer.hlsli"
#include "../Includes/ConstantBuffers/FrameBuffer.hlsli"

Trail_VSOut main(TrailVertex input)
{
    Trail_VSOut output;
    
    float4 viewPos = mul(FB_InvView, input.Position);
    output.Position = viewPos;
    output.Color = input.Color;
    output.Width = input.Width;
    output.Lifetime = input.Lifetime;
    output.ChannelMask = input.ChannelMask;
    
    return output;
}