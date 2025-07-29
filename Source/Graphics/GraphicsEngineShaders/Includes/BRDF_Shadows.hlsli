#include "BRDF_Functions.hlsli"

#ifndef _BRDF_SHADOWS_
#define _BRDF_SHADOWS_


float PenumbraSize(float zReceiver, float zBlocker)
{
    return (zReceiver - zBlocker) /  zBlocker;
}

void FindBlocker(Texture2D shadowMap, float lightSizeUV, float lightNearPlane, out float avgBlockerDepth, out float numBlockers, float2 uv, float zReceiver)
{
    float searchWidth = lightSizeUV * (zReceiver - lightNearPlane) / zReceiver;
    
    float blockerSum = 0;
    numBlockers = 0;
    
    for (int i = 0; i < 16; ++i)
    {
        float shadowMapDepth = shadowMap.SampleLevel(AnisoWrapSampler, uv + PoissonDisk[i] * searchWidth, 0).r;
        
        if (shadowMapDepth < zReceiver)
        {
            blockerSum += shadowMapDepth;
            numBlockers++;
        }
    }
    
    avgBlockerDepth = blockerSum / numBlockers;
}

float PCF_Filter(Texture2D shadowMap, float2 uv, float zReceiver, float filterRadiusUV)
{
    float sum = 0.0f;
    for (int i = 0; i < 16; ++i)
    {
        float2 offset = PoissonDisk[i] * filterRadiusUV;
        sum += shadowMap.SampleCmpLevelZero(ShadowCmpSampler, uv + offset, zReceiver);
    }

    return sum / 16;
}

float PCSS(Texture2D shadowMap, float3 coords, float lightWorldSize, float lightFrustumWidth, float lightNearPlane)
{
    float lightSizeUV = lightWorldSize / lightFrustumWidth;

    float2 uv = coords.xy;
    float zReceiver = coords.z;
    
    float avgBlockerDepth = 0;
    float numBlockers = 0;
    FindBlocker(shadowMap, lightSizeUV, lightNearPlane, avgBlockerDepth, numBlockers, uv, zReceiver);
    
    if (numBlockers == 0)
    {
        return 1.0f;
    }
    else if (numBlockers == 16)
    {
        return 0.0f;
    }
    else
    {
        float penumbraRatio = PenumbraSize(zReceiver, avgBlockerDepth);
        float filterRadiusUV = penumbraRatio * lightSizeUV * lightNearPlane / zReceiver;
    
        return PCF_Filter(shadowMap, uv, zReceiver, filterRadiusUV);
    }
}



void FindBlocker_Cube(TextureCube shadowCubemap, float lightSizeUV, float lightNearPlane, out float avgBlockerDepth, out float numBlockers, float3 direction, float zReceiver)
{
    float searchWidth = lightSizeUV * (zReceiver - lightNearPlane) / zReceiver;
    
    float blockerSum = 0;
    numBlockers = 0;
    
    [unroll(16)]
    for (int i = 0; i < 16; ++i)
    {
        float3 offset = float3(PoissonDisk[i] * searchWidth, 0);
        float shadowMapDepth = shadowCubemap.Sample(AnisoWrapSampler, direction + offset).r;
        
        if (shadowMapDepth < zReceiver)
        {
            blockerSum += shadowMapDepth;
            numBlockers++;
        }
    }
    
    avgBlockerDepth = blockerSum / numBlockers;
}

float PCF_Filter_Cube(TextureCube shadowCubemap, float3 direction, float zReceiver, float filterRadiusUV)
{
    float sum = 0.0f;
    
    [unroll(16)]
    for (int i = 0; i < 16; ++i)
    {
        float3 offset = float3(PoissonDisk[i] * filterRadiusUV, 0);
        sum += shadowCubemap.SampleCmpLevelZero(ShadowCmpSampler, direction + offset, zReceiver);
    }

    return sum / 16;
}

float PCSS_Cube(TextureCube shadowCubemap, float3 direction, float zReceiver, float lightWorldSize, float lightFrustumWidth, float lightNearPlane)
{
    float lightSizeUV = lightWorldSize / lightFrustumWidth;
    
    float avgBlockerDepth = 0;
    float numBlockers = 0;
    FindBlocker_Cube(shadowCubemap, lightSizeUV, lightNearPlane, avgBlockerDepth, numBlockers, direction, zReceiver);
    
    if (numBlockers == 0)
    {
        return 1.0f;
    }
    else if (numBlockers == 16)
    {
        return 0.0f;
    }
    else
    {
        float penumbraRatio = PenumbraSize(zReceiver, avgBlockerDepth);
        float filterRadiusUV = penumbraRatio * lightSizeUV * lightNearPlane / zReceiver;
    
        return PCF_Filter_Cube(shadowCubemap, direction, zReceiver, filterRadiusUV);
    }
}


// Calculating shadows from different types of light sources

float ShadowFactorDLightSpotLight(float4x4 lightView, float4x4 lightProj, float3 lightDir, float4 worldPos, float3 pixelNormal, float minBias, float maxBias, Texture2D shadowMap, float lightWorldSize, float lightFrustumWidth, float lightNearPlane)
{
    float4 lightSpacePos = mul(lightView, worldPos);
    lightSpacePos = mul(lightProj, lightSpacePos);
    float3 lightSpaceUVD = lightSpacePos.xyz / lightSpacePos.w;
    float2 shadowUV = lightSpaceUVD.xy * 0.5f + 0.5f;
    shadowUV.y = 1 - shadowUV.y;
    float bias = max(maxBias * (1 - dot(pixelNormal, normalize(-lightDir))), minBias);
    float D = lightSpaceUVD.z - bias;
    
    // If pixeldepth is beyond shadowcasters farplane, don't bother doing any shadow calculations.
    if (lightSpaceUVD.z > 1.0)
    {
        return 1.0f;
    }
    else
    {
        //return PCSS(shadowMap, float3(shadowUV, D), lightWorldSize, lightFrustumWidth, lightNearPlane);
    
        const int samples = 32;
        float2 shadowMapDim;
        shadowMap.GetDimensions(shadowMapDim.x, shadowMapDim.y);
        float2 texelSize = 1 / shadowMapDim;
        float shadowFactor = 0;
        [unroll(samples)]
        for (int i = 0; i < samples; i++)
        {
            float2 uv = shadowUV + PoissonDisk[i] * texelSize * ((float) samples / 15.0);
            shadowFactor += shadowMap.SampleCmpLevelZero(ShadowCmpSampler, uv, D).r;
        }
        shadowFactor /= samples;
    
        return saturate(shadowFactor);
    }
}

float ShadowFactorPointLight(PointLightData pointLight, float4 worldPos, float3 pixelNormal, float minBias, float maxBias, TextureCube shadowCubemap, float lightWorldSize, float lightFrustumWidth, float lightNearPlane)
{
    float3 pixelToLight = worldPos.xyz - pointLight.Position;
    float distance = max(max(abs(pixelToLight.x), abs(pixelToLight.y)), abs(pixelToLight.z));
    float4 distancePos = float4(0, 0, distance, 1);
    float4 lightSpacePos = mul(pointLight.Projection, distancePos);
    float lightSpaceDepth = lightSpacePos.z / lightSpacePos.w;
    float bias = max(maxBias * (1 - dot(pixelNormal, normalize(pixelToLight))), minBias);
    float D = lightSpaceDepth - bias;
    
    return PCSS_Cube(shadowCubemap, normalize(pixelToLight), D, lightWorldSize, lightFrustumWidth, lightNearPlane);
    
    //float2 shadowMapDim;
    //shadowCubemap.GetDimensions(shadowMapDim.x, shadowMapDim.y);
    //float2 texelSize = 1 / shadowMapDim;
    //float shadowFactor = 0;
    
    //[unroll(64)]
    //for (int i = 0; i < samples; i++)
    //{
    //    float2 directionOffset = pixelToLight.xy + PoissonDisk[i] * texelSize * (float)samples * 30.0;
    //    float3 offsetDirection = float3(directionOffset.xy, pixelToLight.z);
    //    shadowFactor += shadowCubemap.SampleCmpLevelZero(ShadowCmpSampler, offsetDirection, D).r;
    //}
    //shadowFactor /= samples;
    
    //return saturate(shadowFactor);
}

#endif