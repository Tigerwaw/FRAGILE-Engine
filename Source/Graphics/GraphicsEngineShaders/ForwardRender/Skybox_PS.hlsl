#include "../Includes/DefaultShaderIncludes.hlsli"

TextureCube SkyboxTexture : register(t0);

float4 main(Skybox_VS_to_PS input) : SV_TARGET
{
    return float4(SkyboxTexture.SampleLevel(LinearWrapSampler, input.TexCoord, 0).rgb, 1.0f);
}