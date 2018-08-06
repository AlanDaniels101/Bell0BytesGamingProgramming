#pragma once

#pragma region "Description"

/*******************************************************************************************************************************
* Window.h
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
	class DirectXApp;

	class Window
	{
	public:
		Window(DirectXApp* directXApp);
		~Window();

		inline HWND GetMainWindowHandle() { return mainWindow; };

		virtual LRESULT CALLBACK MsgProc(HWND hWnd, unsigned int msg, WPARAM wParam, LPARAM lParam);

		friend class DirectXApp;

	private:
		util::Expected<void> Init();	// initialize the window
		void ReadDesiredResolution();	// get the screen resolution from the config file

	private:
		HWND mainWindow;				// handle to the main window
		DirectXApp* directXApp;			// the core application class

		// Resolution
		int m_clientWidth;
		int m_clientHeight;
	};
}


