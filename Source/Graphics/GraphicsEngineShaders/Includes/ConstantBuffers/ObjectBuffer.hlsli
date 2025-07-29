#ifndef _OBJECT_BUFFER_
#define _OBJECT_BUFFER_

cbuffer ObjectBuffer : register(b1)
{
    float4x4 OB_World;
    bool OB_HasSkinning;
    bool OB_IsInstanced;
    float2 OB_Padding;
    float4 OB_CustomData_1;
    float4 OB_CustomData_2;
}

#endif // _OBJECT_BUFFER_