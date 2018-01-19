#pragma once

#include <opencv2/core.hpp>

#include <ctime>
#include <csignal>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <cmath>

#define FRAME_SKIP 1

template <class T>
struct ObjectEvent
{
public:
	inline ObjectEvent() :
		m_bFlag(false)
	{}

	inline bool WaitGetReset(bool& running, T &value)
	{
		std::unique_lock< std::mutex > lock(m_mutex);
		m_condition.wait(lock, [&]()->bool { return m_bFlag || !running; });
		value = m_content;
		m_bFlag = false;

		return true;
	}

	template< typename R, typename P >
	inline bool WaitGetReset(bool& running, const std::chrono::duration<R, P>& crRelTime, T &value)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		if (!m_condition.wait_for(lock, crRelTime, [&]()->bool { return m_bFlag || !running; }))
		{
			return false;
		}

		value = m_content;
		m_bFlag = false;
		return true;
	}

	inline bool TryGetReset(T &value)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (m_bFlag)
		{
			value = m_content;
			m_bFlag = false;
			return true;
		}

		return false;
	}

	inline bool SetAndSignal(T &value)
	{
		bool bWasSignalled;

		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_content = value;
			bWasSignalled = m_bFlag;
			m_bFlag = true;
		}

		m_condition.notify_all();
		return bWasSignalled == false;
	}

	inline bool Reset()
	{
		bool bWasSignalled;

		{
			std::lock_guard<std::mutex> lock(m_mutex);
			bWasSignalled = m_bFlag;
			m_bFlag = false;
		}

		return bWasSignalled;
	}

	inline bool IsSet() const { return m_bFlag; }

private:
	T m_content;

	bool m_bFlag;
	mutable std::mutex m_mutex;
	mutable std::condition_variable m_condition;
};

struct InputContainer
{
	cv::Mat frame;
};

struct OutputContainer
{
	cv::Mat outputMat;
	int timePF = 1;
	int frameCounter = 0;
	int totalPointsFound = 0;

	cv::Point2f BezPointZero;
	cv::Point2f BezPointOne;
	cv::Point2f BezPointTwo;
	cv::Point2f BezPointThree;
};
