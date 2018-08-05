#pragma region "Description"

/*******************************************************************************************************************************
* Window.cpp
*
********************************************************************************************************************************/

#pragma endregion

#pragma region "Includes"

#include "stdafx.h"

// Project includes
#include "App.h"
#include "ServiceLocator.h"					// Global access to common services
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
		// FOrward messages from this global window procedure to the member window procedure
		return window->MsgProc(hWnd, msg, wParam, lParam);
	}

	Window::Window(DirectXApp* directXApp) :
		mainWindow(NULL),
		directXApp(directXApp)
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
		if (mainWindow)
		{
			mainWindow = NULL;
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
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);				// load the standard application icon
		wc.hIconSm = LoadIcon(0, IDI_APPLICATION);				// load the standard small application icon
		wc.hInstance = directXApp->appInstance;					// handle to the core application instance
		wc.lpfnWndProc = MainWndProc;							// window procedure function
		wc.lpszClassName = L"bell0window";						// class name
		wc.lpszMenuName = 0;									// no menu
		wc.style = CS_HREDRAW | CS_VREDRAW;						// send WM_SIZE message when either the height or the width of the client area are changed
	
		// Register the window
		if (!RegisterClassEx(&wc))
		{
			return std::invalid_argument("The window class could not be registered!");
		}

		// Create the window
		mainWindow = CreateWindowEx(
			WS_EX_OVERLAPPEDWINDOW,		// extended window style
			wc.lpszClassName,			// registered class name
			L"bell0window",				// window name
			WS_OVERLAPPEDWINDOW,		// window style
			CW_USEDEFAULT,				// horizontal position
			CW_USEDEFAULT,				// vertical position
			CW_USEDEFAULT,				// width
			CW_USEDEFAULT,				// height
			NULL,						// parent window handle
			NULL,						// menu / child-window handle
			directXApp->appInstance,	// application instance handle
			NULL						// window creation data
		);
		if (!mainWindow)
		{
			return std::invalid_argument("The window could not be created!");
		}

		// Show and update the window
		ShowWindow(mainWindow, SW_SHOW);
		UpdateWindow(mainWindow);

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
		}

		// Default Windows message handler
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}