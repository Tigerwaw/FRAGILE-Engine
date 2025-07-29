#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/DeferredShaderIncludes.hlsli"
#include "../Includes/ConstantBuffers/MaterialBuffer.hlsli"

GBufferOutput main(MeshVStoPS input)
{
    GBufferOutput output;
    
    float2 UVs = (input.TexCoord0.xy + MB_UVOffset) * MB_UVScale;
    
    const float4 albedoMap = AlbedoTexture.Sample(AnisoWrapSampler, UVs);
    
    if (albedoMap.a < 0.01)
    {
        discard;
    }
    
    const float2 normalMap = NormalTexture.Sample(AnisoWrapSampler, UVs).rg;
    const float3 materialMap = MaterialTexture.Sample(AnisoWrapSampler, UVs).rgb;
    const float4 effectsMap = EffectsTexture.Sample(AnisoWrapSampler, UVs);
    
    float4 adjustedEffects = effectsMap;
    adjustedEffects.r *= MB_EmissiveStrength;
   
    // NORMALS
    float3 calculatedNormals;
    calculatedNormals.xy = (normalMap.xy - 0.5f) * 2;
    calculatedNormals.z = sqrt(1 - saturate(calculatedNormals.x * calculatedNormals.x + calculatedNormals.y * calculatedNormals.y));
    calculatedNormals = normalize(calculatedNormals);
    
    const float3x3 TBN = float3x3(
        normalize(input.Tangent),
        normalize(input.Binormal),
        normalize(input.Normal)
    );
    
    float3 pixelNormal = normalize(mul(calculatedNormals, TBN));
    
    output.Albedo = albedoMap * MB_AlbedoTint;
    output.Material = float4(materialMap.rgb, 0);
    output.Effects = adjustedEffects;
    output.WorldNormal = float4(pixelNormal.rgb, 0);
    output.WorldPosition = input.WorldPos;
    
    return output;
}