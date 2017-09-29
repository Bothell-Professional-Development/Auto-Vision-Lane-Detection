#include "LaneDetector.h"
#include "Geometry.h"
#include <iostream>

LaneDetector::LaneDetector(int32_t verticalRegionUpperY,
                           int32_t verticalRegionLowerY,
                           int32_t horizontalResolution,
                           double convergenceAngle,
                           double convergenceAngleTol) :
    m_verticalRegionUpperY(verticalRegionUpperY),
    m_verticalRegionLowerY(verticalRegionLowerY),
    m_horizontalResolution(horizontalResolution),
    m_laneConvergenceAngle(convergenceAngle),
    m_laneConvergenceAngleTol(convergenceAngleTol)
{
    m_verticalRegionUpper = cv::Vec4i(0, m_verticalRegionUpperY,
                                      m_horizontalResolution, m_verticalRegionUpperY);
    m_verticalRegionLower = cv::Vec4i(0, m_verticalRegionLowerY,
                                      m_horizontalResolution, m_verticalRegionLowerY);
}

std::vector<cv::Vec4i> LaneDetector::FilterHoughLines(std::vector<cv::Vec4i>& houghLines)
{
   /* std::vector<cv::Vec4i> filtered = FilterLinesOutsideOfVerticalRegion(houghLines,
                                                                         m_verticalRegionUpperY,
                                                                         m_verticalRegionLowerY,
                                                                         m_verticalRegionUpper,
                                                                         m_verticalRegionLower);*/
    
	std::vector<cv::Vec4i> filtered = FilterLinesByConvergenceAngle(houghLines, //filtered,
                                             m_laneConvergenceAngle,
                                             m_laneConvergenceAngleTol);

    return filtered;
}

void LaneDetector::SeparateLinesLeftRight(std::vector<cv::Vec4i>& filteredLines, std::vector<cv::Vec4i>& leftLines, std::vector<cv::Vec4i>& rightLines)
{
	for (int i = 0; i < filteredLines.size(); i++)
	{
		cv::Point centerOfLine = FindCenterOfLine(filteredLines[i]);
		if (FindCenterOfLine(filteredLines[i]).x >= m_horizontalResolution / 2.0)
		{
			rightLines.push_back(filteredLines[i]);
		}
		else
		{
			leftLines.push_back(filteredLines[i]);
		}

	}
}

cv::Point LaneDetector::FindCenterOfLine(cv::Vec4i line)
{
	float x, y;
	x = (line[0] + line[2]) / 2.0;
	y = (line[1] + line[3]) / 2.0;
	return cv::Point((int)x, (int)y);
}

std::vector<cv::Vec4i> LaneDetector::FilterLinesByConvergenceAngle(std::vector<cv::Vec4i>& houghLines,
                                                                   double laneConvergenceAngle,
                                                                   double laneConvergenceAngleTol)
{
    std::vector<cv::Vec4i> filtered;

    for (std::vector<cv::Vec4i>::iterator it = houghLines.begin(); it != houghLines.end(); ++it)
    {
        cv::Vec4i current = *it;

        double vecAngleInDegrees = abs(Geometry::RadToDeg(Geometry::GetVectorAngle(current)));
        vecAngleInDegrees = vecAngleInDegrees > 90.0 ? vecAngleInDegrees - 90.0 : vecAngleInDegrees;

        if (vecAngleInDegrees >= laneConvergenceAngle - laneConvergenceAngleTol &&
            vecAngleInDegrees <= laneConvergenceAngle + laneConvergenceAngleTol)
        {
            filtered.push_back(current);
        }
    }

    return filtered;
}

std::vector<cv::Vec4i> LaneDetector::FilterLinesOutsideOfVerticalRegion(std::vector<cv::Vec4i>& houghLines,
                                                                        int32_t verticalRegionUpperY,
                                                                        int32_t verticalRegionLowerY,
                                                                        cv::Vec4i verticalRegionUpper,
                                                                        cv::Vec4i verticalRegionLower)
{
    std::vector<cv::Vec4i> filtered;

    int32_t xOut = -1;
    int32_t yOut = -1;

    for (std::vector<cv::Vec4i>::iterator it = houghLines.begin(); it != houghLines.end(); ++it)
    {
        cv::Vec4i current = const_cast<cv::Vec4i&>(*it);

        //first point above upper vertical region and second point inside vertical region
        if(current[1] < verticalRegionUpperY && 
           (current[3] >= verticalRegionUpperY && current[3] <= verticalRegionLowerY))
        {
            if (Geometry::FindIntersection(current, verticalRegionUpper, xOut, yOut))
            {
                filtered.push_back(cv::Vec4i(current[2], current[3], xOut, yOut));
            }
            else
            {
                //something bad happened
            }
        }
        //second point above upper vertical region and first point inside vertical region
        else if(current[3] < verticalRegionUpperY &&
           (current[1] >= verticalRegionUpperY && current[1] <= verticalRegionLowerY))
        {
            if (Geometry::FindIntersection(current, verticalRegionUpper, xOut, yOut))
            {
                filtered.push_back(cv::Vec4i(current[0], current[1], xOut, yOut));
            }
            else
            {
                //something bad happened
            }
        }
        //first point below lower vertical region and second point inside vertical region
        else if (current[1] > verticalRegionLowerY &&
                 (current[3] >= verticalRegionUpperY && current[3] <= verticalRegionLowerY))
        {
            if (Geometry::FindIntersection(current, verticalRegionLower, xOut, yOut))
            {
                filtered.push_back(cv::Vec4i(current[2], current[3], xOut, yOut));
            }
            else
            {
                //something bad happened
            }
        }
        //second point below lower vertical region and first point inside vertical region
        else if(current[3] > verticalRegionLowerY &&
                (current[1] >= verticalRegionUpperY && current[1] <= verticalRegionLowerY))
        {
            if (Geometry::FindIntersection(current, verticalRegionLower, xOut, yOut))
            {
                filtered.push_back(cv::Vec4i(current[0], current[1], xOut, yOut));
            }
            else
            {
                //something bad happened
            }
        }
        //if line does not merely exist outside of the vertical region
        else if (!(current[1] < verticalRegionUpperY && current[3] < verticalRegionUpperY ||
                   current[1] > verticalRegionLowerY && current[3] > verticalRegionLowerY))
        {
            //if one line endpoint is above upper region line and one is below lower region line, then
            //find intersections to replace original points
            if ((current[1] < verticalRegionUpperY && current[3] > verticalRegionLowerY) ||
                (current[3] < verticalRegionUpperY && current[1] > verticalRegionLowerY))
            {
                int32_t xOut2 = -1;
                int32_t yOut2 = -1;
                
                if (Geometry::FindIntersection(current, verticalRegionUpper, xOut, yOut) &&
                    Geometry::FindIntersection(current, verticalRegionLower, xOut2, yOut2))
                {
                    filtered.push_back(cv::Vec4i(xOut, yOut, xOut2, yOut2));
                }
                else
                {
                    //something bad happened
                }
            }
            //both endpoints are inside of the vertical region
            else
            {
                filtered.push_back(current);
            }
        } 
    }

    return filtered;
}

void LaneDetector::FindLaneMarkerCenterPoints(std::vector<cv::Vec4i>& houghLines, int32_t verticalSamples, std::vector<cv::Point2i>& leftCenterPoints, std::vector<cv::Point2i>& rightCenterPoints)
{
}
