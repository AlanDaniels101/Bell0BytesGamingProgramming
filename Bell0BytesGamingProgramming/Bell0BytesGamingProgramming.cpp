// Bell0BytesGamingProgramming.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Bell0BytesGamingProgramming.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,					// handle to an instance of this application
                     _In_opt_ HINSTANCE hPrevInstance,			// NULL, no longer used
                     _In_ LPWSTR    lpCmdLine,					// command line args
                     _In_ int       nCmdShow)					// how the window is showed
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	MessageBox(NULL, L"Hello World!", L"bell0tutorial", MB_ICONEXCLAMATION | MB_OK);

	return 0;
}