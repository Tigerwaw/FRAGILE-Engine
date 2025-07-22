#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/ConstantBuffers/FrameBuffer.hlsli"
#include "../Includes/ConstantBuffers/ObjectBuffer.hlsli"

Skybox_VS_to_PS main(MeshVertex vertex)
{
    Skybox_VS_to_PS result;
    
    float4 vxLocalPos = vertex.Position;
    float4 vxWorldPos = mul(OB_World, vertex.Position);
    float4 vxViewPos = mul(FB_InvView, vxWorldPos);
    float4 vxProjPos = mul(FB_Projection, vxViewPos);
    
    result.Position = vxProjPos.xyww;
    result.TexCoord = vertex.Position.xyz;
	
    return result;
}