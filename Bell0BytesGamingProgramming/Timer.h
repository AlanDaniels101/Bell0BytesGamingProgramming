#pragma once

#pragma region "Description"

/*******************************************************************************************************************************
* Timer.h
*
* High resolution timer.
*
********************************************************************************************************************************/

#pragma endregion

#pragma region "Includes"

#include "stdafx.h"

// Project includes
#include "Expected.h"

#pragma endregion

namespace core
{
	class Timer
	{
	public:
		Timer();
		~Timer();

		// Getters
		double GetTotalTime() const;	// total runtime
		double GetDeltaTime() const;	// time between frames

		util::Expected<void> Start();
		util::Expected<void> Reset();
		util::Expected<void> Tick();
		util::Expected<void> Stop();

	private:
		// times measured in counts
		long long int startTime;			// time at the start of the application
		long long int totalIdleTime;		// total time the game was idle
		long long int pausedTime;			// time at the moment the game was paused last
		long long int currentTime;			// stores the current time; i.e. time at the current frame
		long long int previousTime;		    // stores the time at the last inquiry before current; i.e. time at the previous frame

		// times measured in seconds
		double secondsPerCount;			    // reciprocal of the frequency, computed once at the initialization of the class
		double deltaTime;					// time between two frames, updated during the game loop

		// state of the timer
		bool isStopped;					    // true if the timer is stopped
	};
}
