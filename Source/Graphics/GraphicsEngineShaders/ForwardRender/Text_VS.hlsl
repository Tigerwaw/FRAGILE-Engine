#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/Sprite_Includes.hlsli"
#include "../Includes/ConstantBuffers/ObjectBuffer.hlsli"

Text_VSout main(TextVertex input)
{
    Text_VSout result;
    float4 worldPos = mul(OB_World, input.Position);
    float4 viewPos = mul(FB_InvView, worldPos);
    result.Position = mul(FB_Projection, viewPos);
    result.TexCoord = input.TexCoord;
	return result;
}