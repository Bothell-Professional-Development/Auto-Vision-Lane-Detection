#include "stdafx.h"

#include <cmath>

#include "Geometry.h"

const float Geometry::FLOAT_TOLERANCE = 1e-4f;

float Geometry::GetVectorAngle(const DirectX::XMFLOAT2 startPoint,
                               const DirectX::XMFLOAT2 endPoint)
{
    float angle = atan2f(endPoint.y - startPoint.y, endPoint.x - startPoint.x);

    return angle >= 0.0f ? angle : angle += DirectX::XM_2PI;
}

float Geometry::FindDistFromCenterToEdgeOfSquare(const float width,
                                                 const float radians)
{
    return width * min(std::abs(Secant(radians)), std::abs(Cosecant(radians)));
}

float Geometry::Secant(const float radians)
{
    return 1.0f / cosf(radians);
}
float Geometry::Cosecant(const float radians)
{
    return 1.0f / sinf(radians);
}

bool Geometry::FindIntersectionBetweenLines(const DirectX::XMFLOAT2 point1,
                                            const DirectX::XMFLOAT2 point2,
                                            const DirectX::XMFLOAT2 point3,
                                            const DirectX::XMFLOAT2 point4,
                                            DirectX::XMFLOAT2& intersection)
{
    bool hasIntersection = true;

    double x1 = point1.x;
    double x2 = point2.x;
    double x3 = point3.x;
    double x4 = point4.x;
    double y1 = point1.y;
    double y2 = point2.y;
    double y3 = point3.y;
    double y4 = point4.y;

    double d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

    // If d is zero, there is no intersection
    if(AlmostEqual(static_cast<float>(d), 0.0f))
    {
        hasIntersection = false;
    }
    else
    {
        double pre = ((x1 * y2) - (y1 *x2));
        double post = ((x3 * y4) - (y3 * x4));
        double x = (pre * (x3 - x4) - (x1 - x2) * post) / d;
        double y = (pre * (y3 - y4) - (y1 - y2) * post) / d;

        // Check if the x and y coordinates on both lines
        if(x + FLOAT_TOLERANCE < min(x1, x2) || x - FLOAT_TOLERANCE > max(x1, x2) ||
           x + FLOAT_TOLERANCE < min(x3, x4) || x - FLOAT_TOLERANCE > max(x3, x4))
        {
            hasIntersection = false;
        }
        else if(y + FLOAT_TOLERANCE < min(y1, y2) || y - FLOAT_TOLERANCE > max(y1, y2) ||
                y + FLOAT_TOLERANCE < min(y3, y4) || y - FLOAT_TOLERANCE > max(y3, y4))
        {
            hasIntersection = false;
        }
        else
        {
            intersection.x = static_cast<float>(x);
            intersection.y = static_cast<float>(y);
        }
    }

    return hasIntersection;
}

bool Geometry::AlmostEqual(const float a,
                           const float b)
{
    return abs(a - b) <= FLOAT_TOLERANCE;
}

float Geometry::Distance(const DirectX::XMFLOAT2 point1,
                         const DirectX::XMFLOAT2 point2)
{
    return hypotf(point2.x - point1.x,
                  point2.y - point1.y);
}