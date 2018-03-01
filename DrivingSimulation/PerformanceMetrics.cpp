#include "stdafx.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "pdh.lib")

#include <mmsystem.h>
//#include <sstream>

#include "PerformanceMetrics.h"

PerformanceMetrics::PerformanceMetrics() :
    m_isProcessorReadable(false),
    m_updateDelay(0),
    m_frameCounter(0),
    m_frameRate(0),
    m_processorUtilization(0),
    m_startTime(0){}

PerformanceMetrics::PerformanceMetrics(const PerformanceMetrics& other){}
PerformanceMetrics::~PerformanceMetrics(){}

void PerformanceMetrics::Initialize(const float updatesPerSecond)
{
    if(updatesPerSecond == 0)
    {
        m_updateDelay = 0;
    }
    else
    {
        m_updateDelay = static_cast<unsigned int>(1000 / updatesPerSecond);
    }
    

    m_startTime = timeGetTime();
    
    m_isProcessorReadable = true;
    PDH_STATUS status = PdhOpenQuery(NULL, 0, &m_queryHandle);

    if(status != ERROR_SUCCESS)
    {
        m_isProcessorReadable = false;
    }
    else
    {
        status = PdhAddCounter(m_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &m_counterHandle);

        if(status != ERROR_SUCCESS)
        {
            m_isProcessorReadable = false;
        }
    }
}

void PerformanceMetrics::Shutdown()
{
    if(m_isProcessorReadable)
    {
        PdhCloseQuery(m_queryHandle);
    }
}

void PerformanceMetrics::Frame()
{
    ++m_frameCounter;

    unsigned int timeNow = timeGetTime();
    unsigned int elapsedTime = timeNow - m_startTime;

    if(elapsedTime >= m_updateDelay)
    {
        m_startTime = timeNow;

        m_frameRate = static_cast<unsigned int>(m_frameCounter / (elapsedTime / 1000.0f));
        m_frameCounter = 0;

        PdhCollectQueryData(m_queryHandle);

        PDH_FMT_COUNTERVALUE value;
        PdhGetFormattedCounterValue(m_counterHandle, PDH_FMT_LONG, NULL, &value);

        m_processorUtilization = value.longValue;
    }
}

const unsigned int& PerformanceMetrics::GetFrameRate() const
{
    return m_frameRate;
}

const unsigned int& PerformanceMetrics::GetProcessorUtilization() const
{
    return m_processorUtilization;
}