#pragma region "Description"

/*******************************************************************************************************************************
* Timer.cpp
*
* High resolution timer.
*
********************************************************************************************************************************/

#pragma endregion

#pragma region "Includes"

#include "stdafx.h"

// Project includes
#include "ServiceLocator.h"
#include "Timer.h"

#pragma endregion


namespace core
{
	Timer::Timer() :
		startTime(0),
		totalIdleTime(0),
		pausedTime(0),
		currentTime(0),
		previousTime(0),
		secondsPerCount(0.0),
		deltaTime(0.0),
		isStopped(false)
	{
		long long int frequency = 0;
		if (QueryPerformanceFrequency((LARGE_INTEGER*)&frequency))
		{
			// Seconds per count is the reciprocal of the frequency
			secondsPerCount = 1.0 / (double)frequency;

#ifndef NDEBUG
			// log success
			util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("The high-precision timer was created successfully.");
#endif
		}
		else
		{
			throw std::runtime_error("The hardware does not support a high-precision timer!");
		}
	}

	Timer::~Timer()
	{
#ifndef NDEBUG
		// log success
		util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("The timer was successfully destroyed.");
#endif
	}

	util::Expected<void> Timer::Start()
	{
		if (isStopped)
		{
			long long int now = 0;
			if (QueryPerformanceCounter((LARGE_INTEGER*)&now))
			{
				// Add the duration of the pause to the total idle time
				totalIdleTime += (now - pausedTime);

				// Set the previous time to the current time
				previousTime = now;

				// Reset the pausedTime to 0 and isStopped to false
				pausedTime = 0;
				isStopped = false;

#ifndef NDEBUG
				util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("The timer was started.");
#endif

				return {};
			}
			else
			{
				return std::runtime_error("Unable to query the performance counter!");
			}
		}

		return {};
	}

	util::Expected<void> Timer::Stop()
	{
		if (!isStopped)
		{
			long long int now = 0;
			if (QueryPerformanceCounter((LARGE_INTEGER*)&now))
			{
				// Set the paused time
				pausedTime = now;
				isStopped = true;

#ifndef NDEBUG
				util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("The timer was stopped.");
#endif

				return {};
			}
			else
			{
				return std::runtime_error("Unable to query the performance counter!");
			}
		}

		return {};
	}

	util::Expected<void> Timer::Reset()
	{
		long long int now = 0;
		if (QueryPerformanceCounter((LARGE_INTEGER*)&now))
		{
			startTime = now;
			previousTime = now;
			pausedTime = 0;
			isStopped = false;

#ifndef NDEBUG
			util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("The timer was reset.");
#endif

			return {};
		}
		else
		{
			return std::runtime_error("Unable to query the performance counter!");
		}
	}

	// Update the delta time between two frames
	util::Expected<void> Timer::Tick()
	{
		if (isStopped)
		{
			deltaTime = 0.0;
			return {};
		}
		else
		{
			if (QueryPerformanceCounter((LARGE_INTEGER*)&currentTime))
			{
				deltaTime = (currentTime - previousTime) * secondsPerCount;

				previousTime = currentTime;

				if (deltaTime < 0.0)
				{
					deltaTime = 0.0;
				}

				return {};
			}
			else
			{
				return std::runtime_error("Unable to query the performance counter!");
			}
		}
	}

	double Timer::GetDeltaTime() const
	{
		return deltaTime;
	}

	double Timer::GetTotalTime() const
	{
		if (isStopped)
		{
			return (pausedTime - startTime - totalIdleTime) * secondsPerCount;
		}
		else
		{
			return (currentTime - startTime - totalIdleTime) * secondsPerCount;
		}
	}
}