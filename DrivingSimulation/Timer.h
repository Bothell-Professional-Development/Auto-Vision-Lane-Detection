#pragma once

#include <Windows.h>

class Timer
{
public:
    Timer();
    Timer(const Timer& other);
    ~Timer();

    bool Initialize();
    void Frame();
    const float GetTimeMilliseconds() const;
    const float& GetTimeSeconds() const;

private:
    long long m_frequency;
    long long m_startTime;
    float m_delta;
};