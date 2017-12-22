#include <math.h>
#include "Geometry.h"

double Geometry::GetVectorAngle(const cv::Vec4i& vector)
{
	return atan2(vector[3] - vector[1], vector[2] - vector[0]);
}


double Geometry::RadToDeg(const double radians)
{
	return radians * 180 / CV_PI;
}

double Geometry::DegToRad(const double degrees)
{
	return degrees * CV_PI / 180;
}

bool Geometry::FindIntersection(const cv::Vec4i& v1,
	const cv::Vec4i& v2,
	int32_t& xOut,
	int32_t& yOut)
{
	bool retVal = true;

	int32_t a0 = v1[3] - v1[1];
	int32_t b0 = v1[0] - v1[2];
	int32_t c0 = v1[2] * v1[1] - v1[0] * v1[3];

	int32_t r2 = a0 * v2[0] + b0 * v2[1] + c0;
	int32_t r3 = a0 * v2[2] + b0 * v2[3] + c0;


	if (r2 != 0 && r3 != 0 && SameSign(r2, r3))
	{
		retVal = false;
	}
	else
	{
		int32_t a1 = v2[3] - v2[1];
		int32_t b1 = v2[0] - v2[2];
		int32_t c1 = v2[2] * v2[1] - v2[0] * v2[3];

		int32_t r0 = a1 * v1[0] + b1 * v1[1] + c1;
		int32_t r1 = a1 * v1[2] + b1 * v1[3] + c1;

		if (r0 != 0 && r2 != 0 && SameSign(r0, r1))
		{
			retVal = false;
		}
		else
		{
			int32_t denom = a0 * b1 - a1 * b0;

			if (denom == 0)
			{
				retVal = false;
			}
			else
			{
				int32_t offset = denom < 0 ? -denom / 2 : denom / 2;

				int32_t num = b0 * c1 - b1 * c0;
				xOut = (num < 0 ? num - offset : num + offset) / denom;

				num = a1 * c0 - a0 * c1;
				yOut = (num < 0 ? num - offset : num + offset) / denom;
			}
		}

	}

	return retVal;
}

bool Geometry::SameSign(const int32_t & x, const int32_t & y)
{
	return (x >= 0) ^ (y < 0);
}
