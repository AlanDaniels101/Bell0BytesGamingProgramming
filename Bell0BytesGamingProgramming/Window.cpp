#pragma region "Description"

/*******************************************************************************************************************************
* Window.cpp
*
* Create a window and handle events.
*
********************************************************************************************************************************/

#pragma endregion

#pragma region "Includes"

#include "stdafx.h"

// Lua and Sol
#include <sol.hpp>

// Resources
#include "resource.h"

// Project includes
#include "App.h"
#include "ServiceLocator.h"					// Global access to common services
#include "StringConverter.h"
#include "Window.h"

#pragma endregion

namespace
{
	core::Window* window = NULL;
}

namespace core
{
	// Window procedure
	LRESULT CALLBACK MainWndProc(HWND hWnd, unsigned int msg, WPARAM wParam, LPARAM lParam)
	{
		// Forward messages from this global window procedure to the member window procedure
		return window->MsgProc(hWnd, msg, wParam, lParam);
	}

	Window::Window(DirectXApp* directXApp) :
		m_hWindow(NULL),
		directXApp(directXApp),
		m_clientWidth(200),
		m_clientHeight(200),
		m_isMinimized(false),
		m_isMaximized(false),
		m_isResizing(false)
	{
		window = this;

		// Initialize the window
		util::Expected<void> ret = this->Init();
		if (!ret.isValid())
		{
			try 
			{ 
				ret.get(); 
			}
			catch (std::exception& e)
			{
				// Log the error
				std::stringstream errorMessage;
				errorMessage << "Creating the game window failed with: " << e.what();
				util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::error>(std::stringstream(errorMessage.str()));
			
				throw std::runtime_error("Window creation failed!");
			}
		}
	}

	Window::~Window()
	{
		if (m_hWindow)
		{
			m_hWindow = NULL;
		}

		if (directXApp)
		{
			directXApp = NULL;
		}

		util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("Main window class destruction was successful.");
	}

	util::Expected<void> Window::Init()
	{
		// specify the window class description
		WNDCLASSEX wc;

		// white window
		wc.cbClsExtra = 0;										// no extra bytes needed
		wc.cbSize = sizeof(WNDCLASSEX);							// size of the window description structure
		wc.cbWndExtra = 0;										// no extra bytes needed
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// brush to repaint the background with
		wc.hCursor = LoadCursor(0, IDC_ARROW);					// load the standard arrow cursor
		wc.hIcon = (HICON)LoadImage(directXApp->m_appInstance, MAKEINTRESOURCE(IDI_BARKING_DOG), IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_DEFAULTCOLOR | LR_SHARED);				// load the barking dog icon
		wc.hIconSm = (HICON)LoadImage(directXApp->m_appInstance, MAKEINTRESOURCE(IDI_BARKING_DOG), IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_DEFAULTCOLOR | LR_SHARED);
		wc.hInstance = directXApp->m_appInstance;				// handle to the core application instance
		wc.lpfnWndProc = MainWndProc;							// window procedure function
		wc.lpszClassName = L"bell0window";						// class name
		wc.lpszMenuName = 0;									// no menu
		wc.style = CS_HREDRAW | CS_VREDRAW;						// send WM_SIZE message when either the height or the width of the client area are changed
	
		// Register the window
		if (!RegisterClassEx(&wc))
		{
			return std::invalid_argument("The window class could not be registered!");
		}

		// Get the screen resolution from the Lua config file
		ReadDesiredResolution();

		// Convert to the necessary window size
		RECT rect = { 0, 0, m_clientWidth, m_clientHeight };
		if (!AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW))
		{
			return std::invalid_argument("The client size of the window could not be computed!");
		}

		// Create the window
		m_hWindow = CreateWindowEx(
			WS_EX_OVERLAPPEDWINDOW,		// extended window style
			wc.lpszClassName,			// registered class name
			L"bell0window",				// window name
			WS_OVERLAPPEDWINDOW,		// window style
			CW_USEDEFAULT,				// horizontal position
			CW_USEDEFAULT,				// vertical position
			rect.right - rect.left,		// width
			rect.bottom - rect.top,		// height
			NULL,						// parent window handle
			NULL,						// menu / child-window handle
			directXApp->m_appInstance,	// application instance handle
			NULL						// window creation data
		);
		if (!m_hWindow)
		{
			return std::invalid_argument("The window could not be created!");
		}

		// Show and update the window
		ShowWindow(m_hWindow, SW_SHOW);
		UpdateWindow(m_hWindow);

		util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("The main window was created successfully.");
		return {};
	}

	// Process message from window event
	LRESULT CALLBACK Window::MsgProc(HWND hWnd, unsigned int msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_DESTROY:
			util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("The main window was flagged for destruction.");
			PostQuitMessage(0);
			return 0;

		case WM_CLOSE:
			// Pause the game
			directXApp->m_isPaused = true;
			directXApp->timer->Stop();

			// Display a confirmation message box
			if (MessageBox(m_hWindow, L"Are you sure you want to quit?", L"Confirmation", MB_YESNO | MB_ICONQUESTION) == IDYES)
				return DefWindowProc(m_hWindow, msg, wParam, lParam);
			else
			{
				// unpause the game
				directXApp->m_isPaused = false;
				directXApp->timer->Start();
				return 0;
			}

		case WM_ACTIVATE:
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				directXApp->m_isPaused = true;
				directXApp->timer->Stop();
			}				
			else
			{
				if (directXApp->m_hasStarted)
				{
					directXApp->timer->Start();
				}
				directXApp->m_isPaused = false;
			}
				
			return 0;

		case WM_MENUCHAR:
			return MAKELRESULT(0, MNC_CLOSE);

		case WM_SIZE:
			if (wParam == SIZE_MINIMIZED)
			{
				m_isMinimized = true;
				m_isMaximized = false;
				directXApp->m_isPaused = true;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				m_isMinimized = false;
				m_isMaximized = true;
				directXApp->OnResize();
				directXApp->m_isPaused = false;
			}
			else if (wParam == SIZE_RESTORED)
			{
				// Window size changed without being minimized or maximized
				if (m_isMinimized)
				{
					m_isMinimized = false;
					directXApp->OnResize();
					directXApp->m_isPaused = false;
				}
				else if (m_isMaximized)
				{
					m_isMaximized = false;
					directXApp->OnResize();
				}
				else if (m_isResizing)
				{
					// Dragging the edge/corner of a window continuously sends WM_SIZE messages. We just want to respond when resizing is done. 
				}
				else
				{
					directXApp->OnResize();
				}
			}
			return 0;

		case WM_ENTERSIZEMOVE:
			// Window is being resized via dragging
			m_isResizing = true;
			directXApp->m_isPaused = true;
			directXApp->timer->Stop();
			return 0;

		case WM_EXITSIZEMOVE:
			// Dragging resize finished
			m_isResizing = false;
			directXApp->OnResize();
			directXApp->m_isPaused = false;
			directXApp->timer->Start();
			return 0;

		case WM_GETMINMAXINFO:
			// Stop the window from being made smaller than 200x200
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
			return 0;

		case WM_KEYDOWN:
			directXApp->OnKeyDown(wParam, lParam);
			return 0;
		}

		// Default Windows message handler
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	void Window::ReadDesiredResolution()
	{
		if (directXApp->m_hasValidConfigurationFile)
		{
			std::wstring pathToPrefsFile = directXApp->m_pathToConfigurationFiles + L"prefs.lua";
		
			try
			{
				sol::state lua;		// sol::state is a managed Lua interface
				lua.script_file(util::StringConverter::ws2s(pathToPrefsFile));

				// Read desired resolution from config file. Default: 200x200
				m_clientWidth = lua["config"]["resolution"]["width"].get_or(200);
				m_clientHeight = lua["config"]["resolution"]["height"].get_or(200);
#ifndef NDEBUG
				std::stringstream res;
				res << "The client resolution was read from the Lua configuration file: " << m_clientWidth << " x " << m_clientHeight << ".";
				util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>(res.str());
#endif
			}
			catch (std::exception)
			{
				util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::warning>("Unable to read configuration file. Starting with default resolution: 200 x 200");
			}
		}
	}
}