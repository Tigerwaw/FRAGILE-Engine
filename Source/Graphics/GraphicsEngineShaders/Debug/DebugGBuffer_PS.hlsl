#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/DeferredShaderIncludes.hlsli"

float4 main(Quad_VS_to_PS input) : SV_TARGET
{    
    switch (FB_DebugRenderMode)
    {
        case 1: // Unlit
            return GBuffer_Albedo.Sample(AnisoWrapSampler, input.UV.xy);
        case 2: // Ambient Occlusion
            return GBuffer_Material.Sample(AnisoWrapSampler, input.UV.xy).r;
        case 3: // Roughness
            return GBuffer_Material.Sample(AnisoWrapSampler, input.UV.xy).g;
        case 4: // Metallic
            return GBuffer_Material.Sample(AnisoWrapSampler, input.UV.xy).b;
        case 5: // Effects
            return GBuffer_Effects.Sample(AnisoWrapSampler, input.UV.xy);
        case 6: // World Normals
            return float4((GBuffer_WorldNormal.Sample(AnisoWrapSampler, input.UV.xy).rgb + 1.0f) * 0.5f, 1.0f);
        case 7: // World Position
            return GBuffer_WorldPos.Sample(AnisoWrapSampler, input.UV.xy);
    }
    
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}