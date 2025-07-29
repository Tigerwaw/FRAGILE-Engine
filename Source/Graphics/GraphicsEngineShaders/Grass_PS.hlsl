#include "Includes/DefaultShaderIncludes.hlsli"
#include "Includes/DeferredShaderIncludes.hlsli"
#include "Includes/ConstantBuffers/MaterialBuffer.hlsli"
#include "Includes/ConstantBuffers/FrameBuffer.hlsli"

GBufferOutput main(MeshVStoPS input, bool isFrontFace : SV_IsFrontFace)
{
    GBufferOutput output;
    
    float2 UVs = (input.TexCoord0.xy + MB_UVOffset) * MB_UVScale;
    
    float4 albedoMap = AlbedoTexture.Sample(AnisoWrapSampler, UVs);
    
    if (albedoMap.a < 0.01)
    {
        discard;
    }
    
    float2 worldUV = input.WorldPos.xz / 5000.0f;
    
    albedoMap.rb *= 0.7;
    float3 variationColor = lerp(0.5f, 5.0f, PerlinNoise.Sample(AnisoWrapSampler, worldUV).r);
    albedoMap.rgb *= variationColor;
    
    float3 windColor = lerp(0.5f, 2.0f, PerlinNoise.Sample(AnisoWrapSampler, worldUV + FB_Time.xx * -0.1f).r);
    albedoMap.rgb *= windColor;
    
    const float2 normalMap = NormalTexture.Sample(AnisoWrapSampler, UVs).rg;
    const float3 materialMap = MaterialTexture.Sample(AnisoWrapSampler, UVs).rgb;
   
    // NORMALS
    float3 calculatedNormals;
    calculatedNormals.xy = (normalMap.xy - 0.5f) * 2;
    calculatedNormals.z = sqrt(1 - saturate(calculatedNormals.x * calculatedNormals.x + calculatedNormals.y * calculatedNormals.y));
    calculatedNormals = normalize(calculatedNormals);
    
    // Calculate normal based on whether we are rendering a frontface or backface.
    int frontFacing = ((int)isFrontFace * 2) - 1;
    
    const float3x3 TBN = float3x3(
        normalize(input.Tangent),
        normalize(input.Binormal),
        normalize(input.Normal * frontFacing)
    );
    
    float3 pixelNormal = normalize(mul(calculatedNormals, TBN));
    
    output.Albedo = albedoMap * MB_AlbedoTint;
    output.Material = float4(materialMap.rgb, 0);
    output.Effects = 0;
    output.WorldNormal = float4(pixelNormal.rgb, 0);
    output.WorldPosition = input.WorldPos;
    
    return output;
}