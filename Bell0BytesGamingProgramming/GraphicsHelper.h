#pragma once

/*******************************************************************************************************************************
* GraphicsHelper.h
*
* Graphics helper functions
*
********************************************************************************************************************************/

#include <wrl/client.h> // Windows and Com

namespace graphics
{
	// Random float between 0.0 and 1.0
	static inline float random0to1()
	{
		return static_cast<float>(rand() / static_cast<float>(RAND_MAX));
	}

	// Random float between 0.0 and 1.0
	static inline float randomColor()
	{
		return random0to1();
	}

	// Random float between -1.0 and 1.0
	static inline float randomPosition()
	{
		return random0to1() * 2 - 1;
	}
}