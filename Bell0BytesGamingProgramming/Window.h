#pragma once

#pragma region "Description"

/*******************************************************************************************************************************
* Window.h
*
* Create a window and handle events.
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
		enum class WindowColor {
			white = 0,
			black,
			grey,
			lightGrey,
			darkGrey,
		};

	public:
		Window(DirectXApp* directXApp, LPCWSTR className, WindowColor windowColor, bool isMainWindow);
		~Window();

		inline HWND GetMainWindowHandle() { return m_hWindow; };

		virtual LRESULT CALLBACK MsgProc(HWND hWnd, unsigned int msg, WPARAM wParam, LPARAM lParam);

		friend class DirectXApp;

	private:
		util::Expected<void> Init(LPCWSTR className, WindowColor windowColor, bool isMainWindow);	// initialize the window
		void ReadDesiredResolution();																// get the screen resolution from the config file

	private:
		HWND m_hWindow;						// handle to the window
		static HWND m_mainWindow;	// handle to the main window
		DirectXApp* directXApp;				// the core application class

		// Resolution
		int m_clientWidth;
		int m_clientHeight;

		// Window state
		bool m_isMinimized;
		bool m_isMaximized;
		bool m_isResizing;
	};
}


