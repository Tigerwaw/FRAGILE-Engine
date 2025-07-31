#include "Includes/DefaultShaderIncludes.hlsli"
#include "Includes/DeferredShaderIncludes.hlsli"
#include "Includes/ConstantBuffers/MaterialBuffer.hlsli"
#include "Includes/ConstantBuffers/FrameBuffer.hlsli"

GBufferOutput main(MeshVStoPS input, bool isFrontFace : SV_IsFrontFace)
{
    GBufferOutput output;
    
    float2 worldUV = input.WorldPos.xz / 5000.0f;
    
    float3 color;
    float3 darkColor = float3(0.243, 0.529, 0.235);
    float3 lightColor = float3(0.075, 0.729, 0.051);
    float3 variationColor = lerp(darkColor, lightColor, PerlinNoise.Sample(AnisoWrapSampler, worldUV).r);
    float3 windColor = lerp(0.25, 1.0, PerlinNoise.Sample(AnisoWrapSampler, worldUV + FB_Time.xx * -0.1f).r * input.VertexColor0.rgb);
    color = variationColor * windColor;
    
    // Calculate normal based on whether we are rendering a frontface or backface.
    int frontFacing = ((int) isFrontFace * 2) - 1;
    
    output.Albedo = float4(color, 1.0f);
    output.Material = float4(1.0f, 1.0f, 0.0f, 0);
    output.Effects = 0;
    output.WorldNormal = float4(normalize(input.Normal * frontFacing), 0);
    output.WorldPosition = input.WorldPos;
    
    return output;
}