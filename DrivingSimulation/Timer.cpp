#include "stdafx.h"

#include "Timer.h"
#include "System.h"

Timer::Timer() :
    m_frequency(0),
    m_startTime(0),
    m_delta(0.0f){}

Timer::Timer(const Timer& other){}
Timer::~Timer(){}

bool Timer::Initialize()
{
    bool success = true;

    LARGE_INTEGER frequency;
    frequency.QuadPart = 0;

    QueryPerformanceFrequency(&frequency);

    if(frequency.QuadPart == 0)
    {
        success = false;
    }
    else
    {
        m_frequency = frequency.QuadPart;
        //m_ticksPerMillisecond = m_frequency / 1000.0f;

        LARGE_INTEGER startTime;
        startTime.QuadPart = 0;

        QueryPerformanceCounter(&startTime);

        if(startTime.QuadPart == 0)
        {
            success = false;
        }
        else
        {
            m_startTime = startTime.QuadPart;
        }
    }

    if(!success)
    {
        System::GetInstance().ShowMessage(L"Timer::Initialize: Failed to initialize timer",
                                          L"Error");
    }

    return success;
}

void Timer::Frame()
{
    LARGE_INTEGER currentTime;

    QueryPerformanceCounter(&currentTime);

    float timeDifference = static_cast<float>(currentTime.QuadPart - m_startTime);
    m_delta = timeDifference / m_frequency;
    m_startTime = currentTime.QuadPart;
}

const float Timer::GetTimeMilliseconds() const
{
    return m_delta * 1000;
}

const float& Timer::GetTimeSeconds() const
{
    return m_delta;
}