#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/DeferredShaderIncludes.hlsli"
#include "../Includes/BRDF_Lights.hlsli"

Texture2D ShadowMapSpot[4] : register(t105);

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
    for (int sIndex = 0; sIndex < LB_NumSpotLights; sIndex++)
    {
        radiance += BRDF_SpotLight(LB_SpotLights[sIndex], FB_ViewPosition.xyz, worldPos, worldNormal, diffuseColor, specularColor, roughness, ShadowMapSpot[sIndex]);
    }
    
    return float4(radiance, 1.0f);
}