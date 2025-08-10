#pragma once
#include <algorithm>
#include <Math/Vector.hpp>

Math::Vector2f HashGradient(int aX, int aY) 
{
    // No precomputed gradients mean this works for any number of grid coordinates
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2;
    unsigned a = aX;
    unsigned b = aY;
    a *= 3284157443;

    b ^= a << s | a >> (w - s);
    b *= 1911520717;

    a ^= b << s | b >> (w - s);
    a *= 2048419325;
    float random = a * (3.14159265f / ~(~0u >> 1)); // in [0, 2*Pi]

    // Create the vector from the angle
    Math::Vector2f v;
    v.x = sin(random);
    v.y = cos(random);

    return v;
}

float DotGridGradient(int aGridX, int aGridY, float aX, float aY)
{
    Math::Vector2f gradient = HashGradient(aGridX, aGridY);

    float dx = aX - static_cast<float>(aGridX);
    float dy = aY - static_cast<float>(aGridY);

    return (dx * gradient.x + dy * gradient.y);
}

float CubicInterpolate(float a0, float a1, float aT)
{
    return (a1 - a0) * (3.0f - aT * 2.0f) * aT * aT + a0;
}

float Perlin(float aX, float aY)
{
    int x0 = static_cast<int>(aX);
    int y0 = static_cast<int>(aY);
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float sx = aX - static_cast<float>(x0);
    float sy = aY - static_cast<float>(y0);

    float n0 = DotGridGradient(x0, y0, aX, aY);
    float n1 = DotGridGradient(x1, y0, aX, aY);
    float interpolatedX0 = CubicInterpolate(n0, n1, sx);

    n0 = DotGridGradient(x0, y1, aX, aY);
    n1 = DotGridGradient(x1, y1, aX, aY);
    float interpolatedX1 = CubicInterpolate(n0, n1, sx);

    float value = CubicInterpolate(interpolatedX0, interpolatedX1, sy);

    return value;
}

float SamplePerlin(Math::Vector2f aCoords, Math::Vector2f aResolution, int aOctaves, float aContrast)
{
    float value = 0.0f;

    float frequency = 1.0f;
    float amplitude = 1.0f;

    for (int i = 0; i < aOctaves; i++)
    {
        value += Perlin(aCoords.x * frequency / aResolution.x, aCoords.y * frequency / aResolution.y) * amplitude;

        frequency *= 2.0f;
        amplitude /= 2.0f;
    }

    value = std::clamp(value * aContrast, -1.0f, 1.0f);
    return value;
}