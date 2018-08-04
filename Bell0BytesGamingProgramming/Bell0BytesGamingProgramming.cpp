#pragma region "Description"

/*******************************************************************************************************************************
* Bell0BytesGamingProgramming.cpp : Defines the entry point for the application.
*
* Sample usage of file logger
* 
********************************************************************************************************************************/

#pragma endregion

#pragma region "Includes"

#include "stdafx.h"

// Project includes
#include "Expected.h"						// Custom exceptions
#include "ServiceLocator.h"					// Global access to common services
#include "Bell0BytesGamingProgramming.h"

#pragma endregion

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,					// handle to an instance of this application
                     _In_opt_ HINSTANCE hPrevInstance,			// NULL, no longer used
                     _In_ LPWSTR    lpCmdLine,					// command line args
                     _In_ int       nCmdShow)					// how the window is showed
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	// Start the logging service
	try 
	{ 
		StartLoggingService();
	}
	catch (std::runtime_error)
	{
		MessageBox(NULL, L"Unable to start logging service!", L"Critical error!", MB_ICONEXCLAMATION | MB_OK);

		return -1;
	}

	return 0;
}

void StartLoggingService()
{
	// Create file logger
	std::shared_ptr<util::Logger<util::FileLogPolicy>> logger(new util::Logger<util::FileLogPolicy>(L"logfile.log"));

	logger->SetThreadName("main");

	// Register logging service
	util::ServiceLocator::ProvideFileLoggingService(logger);

#ifndef NDEBUG
	util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("The file logger was created successfully.");
#endif
}