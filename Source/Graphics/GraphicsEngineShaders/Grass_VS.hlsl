#include "Includes/DefaultShaderIncludes.hlsli"
#include "Includes/ConstantBuffers/AnimationBuffer.hlsli"
#include "Includes/ConstantBuffers/ObjectBuffer.hlsli"
#include "Includes/ConstantBuffers/FrameBuffer.hlsli"

MeshVStoPS main(MeshVertex vertex)
{
    MeshVStoPS result;
    
    float4 localPosition = vertex.Position;
    
    if (OB_IsInstanced)
    {
        localPosition = mul(vertex.RelativeTransform, localPosition);
        result.Normal = mul((float3x3) vertex.RelativeTransform, normalize(vertex.Normal));
        result.Tangent = mul((float3x3) vertex.RelativeTransform, normalize(vertex.Tangent));
        result.Binormal = cross(result.Normal, result.Tangent);
    }
    else
    {
        result.Normal = mul((float3x3) OB_World, normalize(vertex.Normal));
        result.Tangent = mul((float3x3) OB_World, normalize(vertex.Tangent));
        result.Binormal = cross(result.Normal, result.Tangent);
    }
    
    result.WorldPos = mul(OB_World, localPosition);
    float speed = -0.1;
    float magnitude = 50.0;
    float3 noise = PerlinNoise.SampleLevel(AnisoWrapSampler, result.WorldPos.xz + FB_Time.xx * speed, 0).rgb * vertex.VertexColor0.rgb * magnitude;
    
    result.WorldPos.rgb += noise;
    
    result.ViewPos = mul(FB_InvView, result.WorldPos);
    result.Position = mul(FB_Projection, result.ViewPos);
    
    result.VertexColor0 = vertex.VertexColor0;
    result.VertexColor1 = vertex.VertexColor1;
    result.VertexColor2 = vertex.VertexColor2;
    result.VertexColor3 = vertex.VertexColor3;
    result.TexCoord0 = vertex.TexCoord0;
    result.TexCoord1 = vertex.TexCoord1;
    result.TexCoord2 = vertex.TexCoord2;
    result.TexCoord3 = vertex.TexCoord3;
    return result;
}