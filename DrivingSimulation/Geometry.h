#pragma once

#include <DirectXMath.h>

class Geometry
{
public:
    static float GetVectorAngle(const DirectX::XMFLOAT2 startPoint,
                                const DirectX::XMFLOAT2 endPoint);

    static float FindDistFromCenterToEdgeOfSquare(const float width,
                                                  const float radians);

    static float Secant(const float radians);
    static float Cosecant(const float radians);

    static bool FindIntersectionBetweenLines(const DirectX::XMFLOAT2 point1,
                                             const DirectX::XMFLOAT2 point2,
                                             const DirectX::XMFLOAT2 point3,
                                             const DirectX::XMFLOAT2 point4,
                                             DirectX::XMFLOAT2& intersection);

    static bool AlmostEqual(const float a,
                            const float b);

    static float Distance(const DirectX::XMFLOAT2 point1,
                          const DirectX::XMFLOAT2 point2);

    static DirectX::XMFLOAT3 CalculateNormal(const DirectX::XMFLOAT3 vertex1,
                                             const DirectX::XMFLOAT3 vertex2,
                                             const DirectX::XMFLOAT3 vertex3);
private:
    static const float FLOAT_TOLERANCE;
};