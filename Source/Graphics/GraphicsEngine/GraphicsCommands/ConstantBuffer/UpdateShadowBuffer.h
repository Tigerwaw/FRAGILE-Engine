#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"

class UpdateShadowBuffer : public GraphicsCommandBase
{
public:
    struct ShadowData
    {
        Math::Matrix4x4f cameraTransform;
    };

    UpdateShadowBuffer(const ShadowData& aShadowData);
    UpdateShadowBuffer(ShadowData&& aShadowData);
    void Execute() override;
private:
    ShadowData myData;
};

