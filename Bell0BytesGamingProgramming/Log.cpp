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

// Windows
#include <Shlobj.h>
#include <Shlwapi.h>
#include <Pathcch.h>

// Project includes
#include "Log.h"

#pragma endregion

#pragma comment(lib, "ShLwApi.lib")
#pragma comment(lib, "Pathcch.lib")

namespace util
{
	// Open the file stream
	bool FileLogPolicy::OpenOutputStream(const std::wstring& filename)
	{
		PWSTR docPath = NULL;
		HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, NULL, NULL, &docPath);

#ifndef NDEBUG
		if (FAILED(hr))
		{
			return false;
		}
#endif

		// Log file path: "My Documents\Bell0BytesGameProgrammingGuideLogs"
		std::wstringstream path;
		path << docPath << L"\\Bell0BytesGameProgrammingGuideLogs\\";

		::CoTaskMemFree(static_cast<void*>(docPath));

		// Create the directory if it does not exist
		hr = SHCreateDirectory(NULL, path.str().c_str());

#ifndef NDEBUG
		if (FAILED(hr))
		{
			return false;
		}
#endif

		// Append the file name to the file path
		path << filename.c_str();

		// Open the file
		outputStream.open(path.str().c_str(), std::ios_base::binary | std::ios_base::out);

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
