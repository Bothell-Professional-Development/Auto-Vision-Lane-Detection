#ifndef LANE_DETECTOR_H
#define LANE_DETECTOR_H

#include <opencv2/core.hpp>

class LaneDetector
{

public:
    
    LaneDetector(int32_t verticalRegionUpperY,
                 int32_t verticalRegionLowerY,
                 int32_t horizontalResolution,
                 double lineConvergenceAngle,
                 double convergenceAngleTol);

    std::vector<cv::Vec4i> FilterHoughLines(std::vector<cv::Vec4i>& houghLines);
	void SeparateLinesLeftRight(std::vector<cv::Vec4i>& filteredLines, std::vector<cv::Vec4i>& leftLines, std::vector<cv::Vec4i>& rightLines);

private:

    std::vector<cv::Vec4i> FilterLinesByConvergenceAngle(std::vector<cv::Vec4i>& houghLines,
                                                         double laneConvergenceAngle,
                                                         double laneConvergenceAngleTol);
    std::vector<cv::Vec4i> FilterLinesOutsideOfVerticalRegion(std::vector<cv::Vec4i>& houghLines,
                                                              int32_t verticalRegionUpperY,
                                                              int32_t verticalRegionLowerY,
                                                              cv::Vec4i verticalRegionUpper,
                                                              cv::Vec4i verticalRegionLower);
    void FindLaneMarkerCenterPoints(std::vector<cv::Vec4i>& houghLines,
                                    int32_t verticalSamples,
                                    std::vector<cv::Point2i>& leftCenterPoints,
                                    std::vector<cv::Point2i>& rightCenterPoints);
	cv::Point FindCenterOfLine(cv::Vec4i);

    int32_t m_verticalRegionUpperY;
    int32_t m_verticalRegionLowerY;
    int32_t m_horizontalResolution;
    int32_t m_centerLineVerticalSamples;
    double m_laneConvergenceAngle;
    double m_laneConvergenceAngleTol;
    cv::Vec4i m_verticalRegionUpper;
    cv::Vec4i m_verticalRegionLower;
};
#endif
