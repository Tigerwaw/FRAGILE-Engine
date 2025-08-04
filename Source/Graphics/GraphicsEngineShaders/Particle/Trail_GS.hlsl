#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/ParticleIncludes.hlsli"
#include "../Includes/ConstantBuffers/FrameBuffer.hlsli"

[maxvertexcount(4)]
void main(
	line Trail_VSOut input[2], 
	inout TriangleStream<Trail_GSOut> output)
{
    const float2 uvs[4] =
    {
        { 0.0f, 0.0f },
        { 1.0f, 0.0f },
        { 0.0f, 1.0f },
        { 1.0f, 1.0f }
    };
    
    const Trail_VSOut startVertex = input[0];
    const Trail_VSOut endVertex = input[1];
    
    // Topleft vertex
    {
        Trail_GSOut result;
        result.Position = startVertex.Position;
        result.Position.y += startVertex.Width;
        
        result.Position = mul(FB_Projection, result.Position);
        
        result.UV = uvs[0];
        
        result.Color = startVertex.Color;
        result.Lifetime = startVertex.Lifetime;
        result.ChannelMask = startVertex.ChannelMask;
        output.Append(result);
    }
    
    // Bottomleft vertex
    {
        Trail_GSOut result;
        result.Position = startVertex.Position;
        result.Position.y -= startVertex.Width;
        
        result.Position = mul(FB_Projection, result.Position);
        
        result.UV = uvs[1];
        
        result.Color = startVertex.Color;
        result.Lifetime = startVertex.Lifetime;
        result.ChannelMask = startVertex.ChannelMask;
        output.Append(result);
    }
    
    // TopRight vertex
    {
        Trail_GSOut result;
        result.Position = endVertex.Position;
        result.Position.y += endVertex.Width;
        
        result.Position = mul(FB_Projection, result.Position);
        
        result.UV = uvs[2];
        
        result.Color = endVertex.Color;
        result.Lifetime = endVertex.Lifetime;
        result.ChannelMask = endVertex.ChannelMask;
        output.Append(result);
    }
    
    // Bottomright vertex
    {
        Trail_GSOut result;
        result.Position = endVertex.Position;
        result.Position.y -= endVertex.Width;
        
        result.Position = mul(FB_Projection, result.Position);
        
        result.UV = uvs[3];
        
        result.Color = endVertex.Color;
        result.Lifetime = endVertex.Lifetime;
        result.ChannelMask = endVertex.ChannelMask;
        output.Append(result);
    }
}