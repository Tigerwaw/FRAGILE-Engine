#include "DefaultShaderIncludes.hlsli"
#include "ConstantBuffers/LightBuffer.hlsli"

#ifndef _BRDF_INCLUDES_
#define _BRDF_INCLUDES_

#ifndef PI
#define PI 3.14159265358979323846f
#endif

Texture2D BRDF_LUT_Texture : register(t127);

SamplerState BRDF_LUT_Sampler : register(s14);

float NormalDistributionFunction_GGX(float aRoughness, float aNdotH)
{
    float a = aRoughness * aRoughness;
    float aSqr = a * a;
    float nDotHSqr = aNdotH * aNdotH;
    float b = nDotHSqr * (aSqr - 1) + 1;
    float bSqr = b * b;
    return aSqr / (PI * bSqr);
}

float GeometricAttenuation_Smith_GGX(float aNdotX, float aRoughness)
{
    return aNdotX / (aNdotX * (1 - aRoughness) + aRoughness);
}

float GeometricAttenuation_Schlick(float aNdotV, float aNdotL, float aRoughness)
{
    float pixarRoughnessRemap = ((aRoughness + 1) * (aRoughness + 1)) / 8;
    return GeometricAttenuation_Smith_GGX(aNdotL, pixarRoughnessRemap) * GeometricAttenuation_Smith_GGX(aNdotV, pixarRoughnessRemap);
}

float3 Fresnel_SphericalGaussianSchlick(float3 aToView, float3 aHalfangle, float3 aSpecularColor)
{
    float VdotH = saturate(dot(aToView, aHalfangle));
    const float power = ((-5.55473 * VdotH) - 6.98316) * VdotH;
    return aSpecularColor + (1 - aSpecularColor) * pow(2, power);
}

float3 FresnelSchlickRoughness(float cosTheta, float3 aSpecularColor, float roughness)
{
    return float3(0, 0, 0);
}


float3 Diffuse_BRDF(float3 aAlbedo)
{
    return aAlbedo / PI;
}

float3 Specular_BRDF(float aRoughness, float3 aNormal, float3 aHalfAngle, float3 aToView, float3 aToLight, float3 aSpecularColor)
{
    float nDotV = saturate(dot(aNormal, aToView));
    float nDotL = saturate(dot(aNormal, aToLight));
    float nDotH = saturate(dot(aNormal, aHalfAngle));
    float D = NormalDistributionFunction_GGX(aRoughness, nDotH);
    float3 F = Fresnel_SphericalGaussianSchlick(aToView, aHalfAngle, aSpecularColor);
    float G = GeometricAttenuation_Schlick(nDotV, nDotL, aRoughness);
    
    float3 numerator = D * F * G;
    float denominator = 4 * nDotL * nDotV;
    
    return numerator / denominator;
}

int GetNumMips(TextureCube aCube)
{
    int w = 0;
    int h = 0;
    int m = 0;
    aCube.GetDimensions(0, w, h, m);
    return m;
}

float3 Diffuse_IBL(float3 aPixelNormal, TextureCube envCubemap)
{
    const int numMips = GetNumMips(envCubemap) - 1;
    const float3 iblDiffuse = envCubemap.SampleLevel(AnisoWrapSampler, aPixelNormal, numMips).rgb;
    return iblDiffuse;
}

float3 Specular_IBL(float3 aPixelNormal, float3 toViewDir, float aRoughness, float3 aSpecularColor, TextureCube envCubemap)
{
    const int numMips = GetNumMips(envCubemap) - 1;
    const float3 R = reflect(-toViewDir, aPixelNormal);
    
    float3 envColor = envCubemap.SampleLevel(AnisoWrapSampler, R, aRoughness * numMips).rgb;
    
    const float NdotV = saturate(dot(aPixelNormal, toViewDir));
    const float2 brdfLUT = BRDF_LUT_Texture.Sample(BRDF_LUT_Sampler, float2(NdotV, aRoughness)).rg;
    const float3 iblSpecular = envColor * (aSpecularColor * brdfLUT.x + brdfLUT.y);
    return iblSpecular;
}



// Calculating different types of light (Direct & Indirect)

// L = -LightDir for directional light, L = Lightpos - pixelPos for spot & point
float3 BRDF_DirectLighting(float3 viewPos, float3 lightPos, float3 lightColor, float3 L, float3 worldPos, float3 pixelNormal, float3 diffuseColor, float3 specularColor, float roughness)
{
    float3 toView = normalize(viewPos - worldPos);
    float3 toLight = normalize(lightPos - worldPos);
    float3 halfAngle = normalize(normalize(L) + toView);
    float3 kS = Specular_BRDF(roughness, pixelNormal, halfAngle, toView, L, specularColor);
    float3 kD = Diffuse_BRDF(diffuseColor);
    kD *= (1.0f - kS);
    float3 directLight = saturate((kD + kS) * lightColor);
    
    return directLight;
}

float3 BRDF_AmbientLighting(float3 viewPos, float3 diffuseColor, float3 specularColor, float3 pixelNormal, float3 worldPos, float roughness, float ambientOcclusion, TextureCube envCubemap, float3 aAmbientColor, float aAmbientIntensity)
{
    float3 toView = normalize(viewPos - worldPos);
    const float3 diffuseIBL = Diffuse_IBL(pixelNormal, envCubemap);
    const float3 specularIBL = Specular_IBL(pixelNormal, toView, roughness, specularColor, envCubemap);
    const float3 kA = (diffuseColor * diffuseIBL + specularIBL) * ambientOcclusion;
    return kA * aAmbientColor * aAmbientIntensity;
}


// Functions required for convoluting a cubemap.

float2 Hammersley(float i, float numSamples)
{
    uint b = uint(i);

    b = (b << 16u) | (b >> 16u);
    b = ((b & 0x55555555u) << 1u) | ((b & 0xAAAAAAAAu) >> 1u);
    b = ((b & 0x33333333u) << 2u) | ((b & 0xCCCCCCCCu) >> 2u);
    b = ((b & 0x0F0F0F0Fu) << 4u) | ((b & 0xF0F0F0F0u) >> 4u);
    b = ((b & 0x00FF00FFu) << 8u) | ((b & 0xFF00FF00u) >> 8u);

    const float radicalInverseVDC = float(b) * 2.3283064365386963e-10;
    return float2((i / numSamples), radicalInverseVDC);
}

float3 ImportanceSampleGGX(float2 aXi, float3 aNormal, float aRoughness)
{
    const float roughnesSq = aRoughness * aRoughness;

    const float phi = 2.0f * PI * aXi.x;
    const float cosTheta = sqrt((1.0f - aXi.y) / (1.0f + (roughnesSq * roughnesSq - 1) * aXi.y));
    const float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

    float3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    const float3 up = abs(aNormal.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
    const float3 tangent = normalize(cross(up, aNormal));
    const float3 bitangent = cross(aNormal, tangent);

    return normalize(tangent * H.x + bitangent * H.y + aNormal * H.z);
}





// LUT creation function for caculating a BRDF lookup for the second half of the split-sum equation.
// Run this once with float2 result = IntegrateBRDF(uv.x, uv.y); Causes a LUT to be generated.

float GeometricAttenuation_Schlick_GGX_IBL(float aNdotV, float aRoughness)
{
    // Note different k here when calculating G GGX for IBL!
    const float a = aRoughness;
    const float k = (a * a) / 2.0f;

    const float nominator = aNdotV;
    const float denominator = aNdotV * (1.0 - k) + k;

    return nominator / denominator;
}

float GeometricAttenuation_Smith_IBL(float3 aN, float3 aV, float3 aL, float aRoughness)
{
    const float NdotV = saturate(dot(aN, aV));
    const float NdotL = saturate(dot(aN, aL));
    
    const float GGX_NdotV = GeometricAttenuation_Schlick_GGX_IBL(NdotV, aRoughness);
    const float GGX_NdotL = GeometricAttenuation_Schlick_GGX_IBL(NdotL, aRoughness);

    return GGX_NdotL * GGX_NdotV;
}

float2 IntegrateBRDF(float aNdotV, float aRoughness)
{
    float3 V;
    V.x = sqrt(1.0f - aNdotV * aNdotV);
    V.y = 0.0;
    V.z = aNdotV;

    float a = 0;
    float b = 0;

    const float3 N = float3(0, 0, 1);

    const uint NUM_SAMPLES = 1024u;
    for (uint i = 0u; i < NUM_SAMPLES; ++i)
    {
        const float2 xi = Hammersley(i, NUM_SAMPLES);
        const float3 H = ImportanceSampleGGX(xi, N, aRoughness);
        const float3 L = normalize(2.0 * dot(V, H) * H - V);

        const float NdotL = saturate(L.z);
        const float NdotH = saturate(H.z);
        const float VdotH = saturate(dot(V, H));
        const float NdotV = saturate(dot(N, V));

        if (NdotL > 0.0)
        {
            const float G = GeometricAttenuation_Smith_IBL(N, V, L, aRoughness);
            const float G_Vis = (G * VdotH) / (NdotH * NdotV);
            const float Fc = pow(1.0 - VdotH, 5.0);

            a += (1.0 - Fc) * G_Vis;
            b += Fc * G_Vis;
        }
    }

    return float2(a, b) / NUM_SAMPLES;
}

#endif // _BRDF_INCLUDES_