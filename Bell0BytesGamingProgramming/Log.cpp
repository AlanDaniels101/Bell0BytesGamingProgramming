#pragma region "Description"

/*******************************************************************************************************************************
* Logging utility
*
* Log daemon, log interface, severity definitions, file logger
*
********************************************************************************************************************************/

#pragma endregion

#pragma region "Includes"

#include "stdafx.h"

// Project includes
#include "Log.h"

#pragma endregion

namespace util
{
	// Open the file stream
	bool FileLogPolicy::OpenOutputStream(const std::wstring& filename)
	{
		// Open the file
		outputStream.open(filename.c_str(), std::ios_base::binary | std::ios_base::out);

#ifndef NDEBUG
		if (!outputStream.is_open())
		{
			return false;
		}
#endif

		// set output precision
		outputStream.precision(20);

		// return success
		return true;
	}

	// Close the file stream
	void FileLogPolicy::CloseOutputStream()
	{
		outputStream.close();
	}

	// Write a message to the log file
	void FileLogPolicy::Write(const std::string& msg)
	{
		outputStream << msg << std::endl;
	}
}
