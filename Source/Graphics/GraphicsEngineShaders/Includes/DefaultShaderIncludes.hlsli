#include "ConstantBuffers/FrameBuffer.hlsli"

#ifndef _DEFAULT_INCLUDES_
#define _DEFAULT_INCLUDES_

Texture2D AlbedoTexture : register(t0);
Texture2D NormalTexture : register(t1);
Texture2D MaterialTexture : register(t2);
Texture2D EffectsTexture : register(t3);

SamplerState LinearWrapSampler : register(s0);
SamplerState LinearClampSampler : register(s1);
SamplerState PointWrapSampler : register(s2);
SamplerState PointClampSampler : register(s3);
SamplerComparisonState ShadowCmpSampler : register(s15);

Texture2D PerlinNoise : register(t50);
Texture2D BlueNoise : register(t51);

struct MeshVertex
{
    float4 Position             : POSITION;
    float4 VertexColor0         : VERTEXCOLOR0;
    float4 VertexColor1         : VERTEXCOLOR1;
    float4 VertexColor2         : VERTEXCOLOR2;
    float4 VertexColor3         : VERTEXCOLOR3;
    float2 TexCoord0            : TEXCOORD0;
    float2 TexCoord1            : TEXCOORD1;
    float2 TexCoord2            : TEXCOORD2;
    float2 TexCoord3            : TEXCOORD3;
    float3 Normal               : NORMAL;
    float3 Tangent              : TANGENT;
    uint4 BoneIDs               : BONEIDS;
    float4 Skinweights          : SKINWEIGHTS;
    float4x4 RelativeTransform  : RELATIVEXF;
};

struct MeshVStoPS
{
    float4 Position     : SV_POSITION;
    float4 WorldPos     : WORLDPOSITION;
    float4 ViewPos      : VIEWPOSITION;
    float4 VertexColor0 : VERTEXCOLOR0;
    float4 VertexColor1 : VERTEXCOLOR1;
    float4 VertexColor2 : VERTEXCOLOR2;
    float4 VertexColor3 : VERTEXCOLOR3;
    float2 TexCoord0    : TEXCOORD0;
    float2 TexCoord1    : TEXCOORD1;
    float2 TexCoord2    : TEXCOORD2;
    float2 TexCoord3    : TEXCOORD3;
    float3 Normal       : NORMAL;
    float3 Tangent      : TANGENT;
    float3 Binormal     : BINORMAL;
};

struct ShadowCube_VSout
{
    float4 Position     : POSITION;
};

struct ShadowCube_GSout
{
    float4 Position     : SV_POSITION;
    uint RTIndex        : SV_RenderTargetArrayIndex;
};

struct Quad_VS_to_PS
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD;
};

struct Skybox_VS_to_PS
{
    float4 Position : SV_Position;
    float3 TexCoord : TEXCOORD;
};

static const float2 defaultUVs[4] =
{
    float2(0.0f, 1.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 1.0f),
    float2(1.0f, 0.0f)
};

static const float2 PoissonDisk[64] =
{
    float2(-0.613392, 0.617481),
    float2(0.170019, -0.040254),
    float2(-0.299417, 0.791925),
    float2(0.645680, 0.493210),
    float2(-0.651784, 0.717887),
    float2(0.421003, 0.027070),
    float2(-0.817194, -0.271096),
    float2(-0.705374, -0.668203),
    float2(0.977050, -0.108615),
    float2(0.063326, 0.142369),
    float2(0.203528, 0.214331),
    float2(-0.667531, 0.326090),
    float2(-0.098422, -0.295755),
    float2(-0.885922, 0.215369),
    float2(0.566637, 0.605213),
    float2(0.039766, -0.396100),
    float2(0.751946, 0.453352),
    float2(0.078707, -0.715323),
    float2(-0.075838, -0.529344),
    float2(0.724479, -0.580798),
    float2(0.222999, -0.215125),
    float2(-0.467574, -0.405438),
    float2(-0.248268, -0.814753),
    float2(0.354411, -0.887570),
    float2(0.175817, 0.382366),
    float2(0.487472, -0.063082),
    float2(-0.084078, 0.898312),
    float2(0.488876, -0.783441),
    float2(0.470016, 0.217933),
    float2(-0.696890, -0.549791),
    float2(-0.149693, 0.605762),
    float2(0.034211, 0.979980),
    float2(0.503098, -0.308878),
    float2(-0.016205, -0.872921),
    float2(0.385784, -0.393902),
    float2(-0.146886, -0.859249),
    float2(0.643361, 0.164098),
    float2(0.634388, -0.049471),
    float2(-0.688894, 0.007843),
    float2(0.464034, -0.188818),
    float2(-0.440840, 0.137486),
    float2(0.364483, 0.511704),
    float2(0.034028, 0.325968),
    float2(0.099094, -0.308023),
    float2(0.693960, -0.366253),
    float2(0.678884, -0.204688),
    float2(0.001801, 0.780328),
    float2(0.145177, -0.898984),
    float2(0.062655, -0.611866),
    float2(0.315226, -0.604297),
    float2(-0.780145, 0.486251),
    float2(-0.371868, 0.882138),
    float2(0.200476, 0.494430),
    float2(-0.494552, -0.711051),
    float2(0.612476, 0.705252),
    float2(-0.578845, -0.768792),
    float2(-0.772454, -0.090976),
    float2(0.504440, 0.372295),
    float2(0.155736, 0.065157),
    float2(0.391522, 0.849605),
    float2(-0.620106, -0.328104),
    float2(0.789239, -0.419965),
    float2(-0.545396, 0.538133),
    float2(-0.178564, -0.596057)
};

static const float GaussianKernel[5] = 
    { 
        0.06136f, 
        0.24477f, 
        0.38774f, 
        0.24477f, 
        0.06136 
    };

float RandomNumber(float4 seed)
{
    float dot_product = dot(seed, float4(12.9898, 78.233, 45.164, 94.673));
    return frac(sin(dot_product) * 43758.5453);
}

float GetLogDepth(float4 worldPosition)
{
    float4 cameraPos = mul(FB_InvView, worldPosition);
    float4 projPos = mul(FB_Projection, cameraPos);
    return projPos.z / projPos.w;
}

float RemapRange(float aValue, float aStartingLowerBound, float aStartingUpperBound, float aEndingLowerBound, float aEndingUpperBound)
{
    float startDiff = aStartingUpperBound - aStartingLowerBound;
    float endDiff = aEndingUpperBound - aEndingLowerBound;
    float remappedValue = aEndingLowerBound + (aValue - aStartingLowerBound) * endDiff / startDiff;
    return remappedValue;
}

// Size is rows & columns
// Progress is what frame in the animation we are on
float2 Flipbook(float2 uv, float2 size, float progress)
{
    progress = floor(fmod(progress, (size.x * size.y)));
    float2 frameSize = float2(1.0, 1.0) / size;
    float2 frame = frac(uv / size) + frameSize;
    
    frame.x += ((progress / size.x) - frameSize.x * floor(progress / size.x) * size.x) - frameSize.x;
    frame.y += (frameSize.y * floor(progress / size.x)) - frameSize.y;

    return frame;
}

float3 GetRandomNoise(float2 UV, float2 aNoiseScale)
{
    return BlueNoise.SampleLevel(PointWrapSampler, UV * aNoiseScale, 0).rgb;
}

float2 GetNoiseScale()
{
    uint w, h, m;
    BlueNoise.GetDimensions(0, w, h, m);
    float2 noiseScale = float2(w, h);
    return FB_Resolution / noiseScale;
}

float2 Rotate2DPointAroundPivot(float2 aPivot, float2 aPoint, float aAngleInRad)
{
    float s = sin(aAngleInRad);
    float c = cos(aAngleInRad);
        
    aPoint.x -= aPivot.x;
    aPoint.y -= aPivot.y;
        
    float xnew = aPoint.x * c - aPoint.y * s;
    float ynew = aPoint.x * s + aPoint.y * c;
        
    aPoint.x = xnew + aPivot.x;
    aPoint.y = ynew + aPivot.y;
    
    return aPoint;
}

#endif // _DEFAULT_INCLUDES_