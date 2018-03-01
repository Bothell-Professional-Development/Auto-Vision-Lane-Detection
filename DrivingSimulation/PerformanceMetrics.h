#pragma once

#include "pdh.h"

class PerformanceMetrics
{
public:
    PerformanceMetrics();
    PerformanceMetrics(const PerformanceMetrics& other);
    ~PerformanceMetrics();

    void Initialize(const float updatesPerSecond);

    void Shutdown();
    void Frame();

    const unsigned int& GetFrameRate() const;
    const unsigned int& GetProcessorUtilization() const;

private:

    bool m_isProcessorReadable;

    unsigned int m_updateDelay;
    unsigned int m_frameCounter;
    unsigned int m_frameRate;
    unsigned int m_processorUtilization;

    unsigned long m_startTime;

    HQUERY m_queryHandle;
    HCOUNTER m_counterHandle;
};