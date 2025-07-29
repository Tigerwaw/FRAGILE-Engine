#include "../Includes/DefaultShaderIncludes.hlsli"

float4 main(MeshVStoPS input) : SV_TARGET
{
    switch (FB_DebugRenderMode)
    {
        case 8: // Wireframe
            return float4(0.0f, 1.0f, 0.0f, 1.0f);
        case 9: // Vertex Normals
            return float4((input.Normal + 1.0f) * 0.5f, 1.0f);
        case 10: // Texture Normals
        {
            const float2 normalMap = NormalTexture.Sample(AnisoWrapSampler, input.TexCoord0.xy).rg;
            float3 calculatedNormals;
            calculatedNormals.xy = (normalMap.xy - 0.5f) * 2;
            calculatedNormals.z = sqrt(1 - saturate(calculatedNormals.x * calculatedNormals.x + calculatedNormals.y * calculatedNormals.y));
            calculatedNormals = (calculatedNormals + 1) * 0.5;
            return float4(calculatedNormals, 1.0f);
        }
        case 11: // Vertex Colors
            return input.VertexColor0;
        case 12: // UVs
            return float4(input.TexCoord0.xy, 0.0f, 1.0f);
    }
    
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}