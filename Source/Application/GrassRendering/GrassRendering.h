#pragma once
#include "GameEngine/Application/Application.h"
#include "GameEngine/Application/EntryPoint.h"

class GrassRendering : public Application
{
public:
    void InitializeApplication() override;
    void UpdateApplication() override;
    void UpdateDebug() override;

private:
    Math::Matrix4x4f CalculateInstanceTransform(int aInner, int aOuter, int aChunkHorizontalIndex, int aChunkVerticalIndex, Math::Vector3f aCurrentChunkOffset) const;

private:
    unsigned currentDebugMode = 0;

    const float myDensity = 2.0f;
    const float myChunkOffset = 1000.0f;
    const float myYRayOrigin = 1500.0f;
    const float mySizeVariation = 0.25f;

    const int myInstanceRows = static_cast<int>(std::roundf(100.0f * myDensity));
    const float myInstanceOffset = myChunkOffset / myInstanceRows;
    const float myInstanceOffsetVariation = myInstanceOffset * 0.8f;
};