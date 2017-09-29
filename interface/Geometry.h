#ifndef LANE_GEOMETRY_H
#define LANE_GEOMETRY_H

#include <opencv2/core.hpp>

class Geometry
{
public:

    static double GetVectorAngle(const cv::Vec4i& vector);
    static double RadToDeg(const double radians);
    static double DegToRad(const double degrees);
    static bool FindIntersection(const cv::Vec4i& v1,
                                 const cv::Vec4i& v2,
                                 int32_t& xOut,
                                 int32_t& yOut);

private:
    static bool SameSign(const int32_t& x,
                         const int32_t& y);
};

#endif