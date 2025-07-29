#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/DeferredShaderIncludes.hlsli"
#include "../Includes/BRDF_Lights.hlsli"
#include "../Includes/ConstantBuffers/PostProcessBuffer.hlsli"

TextureCube EnvCubeMap : register(t126);
Texture2D ShadowMapDir : register(t100);

Texture2D SSAOTexture : register(t30);

float4 main(Quad_VS_to_PS input) : SV_TARGET
{
    float3 albedoColor = GBuffer_Albedo.Sample(AnisoWrapSampler, input.UV).rgb;
    float3 material = GBuffer_Material.Sample(AnisoWrapSampler, input.UV).rgb;
    float4 effects = GBuffer_Effects.Sample(AnisoWrapSampler, input.UV);
    float3 worldNormal = GBuffer_WorldNormal.Sample(AnisoWrapSampler, input.UV).rgb;
    float4 worldPos = GBuffer_WorldPos.Sample(AnisoWrapSampler, input.UV);
    
    float ambientOcclusion = material.r;
    
    if (PPB_SSAOEnabled)
    {
        const float ssaoMap = SSAOTexture.Sample(LinearClampSampler, input.UV).r;
        ambientOcclusion = min(ssaoMap, material.r);
    }
    
    const float roughness = material.g;
    const float metalness = material.b;
    
    const float3 emission = albedoColor * effects.r;

    const float3 specularColor = lerp((float3) 0.04f, albedoColor.rgb, metalness);
    const float3 diffuseColor = lerp((float3) 0.00f, albedoColor.rgb, 1 - metalness);
    
    float3 ambientLight = BRDF_AmbientLighting(FB_ViewPosition.xyz, diffuseColor, specularColor, worldNormal, worldPos.rgb, roughness, ambientOcclusion, EnvCubeMap, LB_AmbientLight.Color, LB_AmbientLight.Intensity);
    float3 directionalLightRadiance = BRDF_DirectionalLight(LB_DirLight, FB_ViewPosition.xyz, worldPos, worldNormal, diffuseColor, specularColor, roughness, ShadowMapDir);
    
    float3 radiance = ambientLight + directionalLightRadiance + emission;
    
    return float4(radiance, 1.0f);
}