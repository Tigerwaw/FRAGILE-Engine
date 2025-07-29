#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/BRDF_Lights.hlsli"
#include "../Includes/ConstantBuffers/FrameBuffer.hlsli"
#include "../Includes/ConstantBuffers/LightBuffer.hlsli"
#include "../Includes/ConstantBuffers/MaterialBuffer.hlsli"

TextureCube EnvCubeMap : register(t126);

Texture2D ShadowMapDir : register(t100);
TextureCube ShadowMapPoint[4] : register(t101);
Texture2D ShadowMapSpot[4] : register(t105);

float4 main(MeshVStoPS input) : SV_TARGET
{
    const float4 albedoMap = AlbedoTexture.Sample(AnisoWrapSampler, input.TexCoord0.xy) * MB_AlbedoTint;
    
    if (albedoMap.a < 0.01)
    {
        discard;
        return 0;
    }
    
    const float2 normalMap = NormalTexture.Sample(AnisoWrapSampler, input.TexCoord0.xy).rg;
    const float3 materialMap = MaterialTexture.Sample(AnisoWrapSampler, input.TexCoord0.xy).rgb;
    const float4 effectsMap = EffectsTexture.Sample(AnisoWrapSampler, input.TexCoord0.xy);
    
    float3 emission = albedoMap.rgb * effectsMap.r * MB_EmissiveStrength;
   
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
    
    
    // LIGHTING
    const float ambientOcclusion = materialMap.r;
    const float roughness = materialMap.g;
    const float metalness = materialMap.b;
    
    const float3 specularColor = lerp((float3) 0.04f, albedoMap.rgb, metalness);
    const float3 diffuseColor = lerp((float3) 0.00f, albedoMap.rgb, 1 - metalness);
    
    float3 ambientLight = BRDF_AmbientLighting(FB_ViewPosition.xyz, diffuseColor, specularColor, pixelNormal, input.WorldPos.xyz, roughness, ambientOcclusion, EnvCubeMap, LB_AmbientLight.Color, LB_AmbientLight.Intensity);
    float3 directionalLightRadiance = BRDF_DirectionalLight(LB_DirLight, FB_ViewPosition.xyz, input.WorldPos, pixelNormal, diffuseColor, specularColor, roughness, ShadowMapDir);
    
    float3 pointLightRadiance = 0;
    [unroll(4)]
    for (int pIndex = 0; pIndex < LB_NumPointLights; pIndex++)
    {
        pointLightRadiance += BRDF_PointLight(LB_PointLights[pIndex], FB_ViewPosition.xyz, input.WorldPos, pixelNormal, diffuseColor, specularColor, roughness, ShadowMapPoint[pIndex]);
    }
    
    float3 spotLightRadiance = 0;
    [unroll(4)]
    for (int sIndex = 0; sIndex < LB_NumSpotLights; sIndex++)
    {
        spotLightRadiance += BRDF_SpotLight(LB_SpotLights[sIndex], FB_ViewPosition.xyz, input.WorldPos, pixelNormal, diffuseColor, specularColor, roughness, ShadowMapSpot[sIndex]);
    }
    
    float3 radiance = ambientLight + directionalLightRadiance + pointLightRadiance + spotLightRadiance;
    
    float4 color = 1;
    color.rgb = radiance;
    color.a = albedoMap.a;
    
    return color;
}