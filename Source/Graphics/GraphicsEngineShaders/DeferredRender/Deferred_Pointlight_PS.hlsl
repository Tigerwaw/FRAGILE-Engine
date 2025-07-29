#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/DeferredShaderIncludes.hlsli"
#include "../Includes/BRDF_Lights.hlsli"

TextureCube ShadowMapPoint[4] : register(t101);

float4 main(Quad_VS_to_PS input) : SV_TARGET
{
    float3 albedoColor = GBuffer_Albedo.Sample(AnisoWrapSampler, input.UV).rgb;
    float3 material = GBuffer_Material.Sample(AnisoWrapSampler, input.UV).rgb;
    float3 worldNormal = GBuffer_WorldNormal.Sample(AnisoWrapSampler, input.UV).rgb;
    float4 worldPos = GBuffer_WorldPos.Sample(AnisoWrapSampler, input.UV);
    
    const float ambientOcclusion = material.r;
    const float roughness = material.g;
    const float metalness = material.b;

    const float3 specularColor = lerp((float3) 0.04f, albedoColor.rgb, metalness);
    const float3 diffuseColor = lerp((float3) 0.00f, albedoColor.rgb, 1 - metalness);
    
    float3 radiance = 0;
    [unroll(4)]
    for (int pIndex = 0; pIndex < LB_NumPointLights; pIndex++)
    {
        radiance += BRDF_PointLight(LB_PointLights[pIndex], FB_ViewPosition.xyz, worldPos, worldNormal, diffuseColor, specularColor, roughness, ShadowMapPoint[pIndex]);
    }
    
    return float4(radiance, 1.0f);
}