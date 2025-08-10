#include "Includes/DefaultShaderIncludes.hlsli"
#include "Includes/ConstantBuffers/AnimationBuffer.hlsli"
#include "Includes/ConstantBuffers/ObjectBuffer.hlsli"
#include "Includes/ConstantBuffers/FrameBuffer.hlsli"

MeshVStoPS main(MeshVertex vertex, uint instanceID : SV_InstanceID)
{
    MeshVStoPS result;
    
    float4 localPosition = mul(vertex.RelativeTransform, vertex.Position);
    float3 localNormal = mul((float3x3) vertex.RelativeTransform, normalize(vertex.Normal));
    float3 localTangent = mul((float3x3) vertex.RelativeTransform, normalize(vertex.Tangent));
    
    result.Normal = mul((float3x3) OB_World, localNormal);
    result.Tangent = mul((float3x3) OB_World, localTangent);
    result.Binormal = cross(result.Normal, result.Tangent);
    result.WorldPos = mul(OB_World, localPosition);
    
    float hashedID = Random(instanceID);
    float frequency = 1.0;
    float amplitude = 10.0;
    float instanceAmplitude = 0.5;
    
    float height = vertex.RelativeTransform._22 * hashedID;
    float wave = cos(FB_Time.x * (frequency - height));
    
    wave = (wave * wave * amplitude) - hashedID * instanceAmplitude;
    float sway = wave * vertex.VertexColor0.r * height;
    result.WorldPos.rb += sway;
    
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