#pragma region "Description"

/*******************************************************************************************************************************
* Bell0BytesGamingProgramming.cpp : Defines the entry point for the application.
*
* Sample usage of Expected<T>
* 
********************************************************************************************************************************/

#pragma endregion

#pragma region "Includes"

#include "stdafx.h"

// Project includes
#include "Expected.h" // Custom exceptions
#include "Bell0BytesGamingProgramming.h"

#pragma endregion

util::Expected<void> foo(int x)
{
	if (x != 9)
		return std::invalid_argument("I only like the number 9!");

	return {};
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,					// handle to an instance of this application
                     _In_opt_ HINSTANCE hPrevInstance,			// NULL, no longer used
                     _In_ LPWSTR    lpCmdLine,					// command line args
                     _In_ int       nCmdShow)					// how the window is showed
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	if (foo(4).isValid())
		return 0;
	else
		return -1;
}