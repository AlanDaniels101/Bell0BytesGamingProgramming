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

namespace graphics
{
	class Direct3D;
}

namespace core
{
	class DirectXApp;

	class Window
	{
	public:
		friend class DirectXApp;
		friend class graphics::Direct3D;

		Window(DirectXApp* directXApp);
		~Window();

		inline HWND GetMainWindowHandle() { return m_hWindow; };

		virtual LRESULT CALLBACK MsgProc(HWND hWnd, unsigned int msg, WPARAM wParam, LPARAM lParam);

	private:
		util::Expected<void> Init();		// initialize the window
		void ReadDesiredResolution();		// get the screen resolution from the config file

	private:
		HWND m_hWindow;						// handle to the window
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


