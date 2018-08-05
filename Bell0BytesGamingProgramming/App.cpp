#pragma once

#pragma region "Description"

/*******************************************************************************************************************************
* App.cpp : The main game class
*
********************************************************************************************************************************/

#pragma endregion

#pragma region "Includes"

#include "stdafx.h"

// Project includes
#include "ServiceLocator.h"					// Global access to common services
#include "App.h"

#pragma endregion

namespace core
{
	DirectXApp::DirectXApp(HINSTANCE hInstance) :
		appInstance(hInstance),
		appWindow(NULL)
	{}

	DirectXApp::~DirectXApp()
	{
		Shutdown();
	}

	util::Expected<void> DirectXApp::Init()
	{
		// Create the application window
		try 
		{
			appWindow = new Window(this);
		}
		catch (std::runtime_error)
		{
			return std::runtime_error("DirectXApp was unable to create the main window!");
		}

		// log and return success
		util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("The DirectX application initialization was successful.");
		return {};
	}

	void DirectXApp::Shutdown(util::Expected<void>* expected)
	{
		if (appWindow)
		{
			delete appWindow;
		}

		if (appInstance)
		{
			appInstance = NULL;
		}
	
		util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("The DirectX application was shutdown successfully.");
	}

	// Main event loop
	util::Expected<int> DirectXApp::Run()
	{
		bool continueRunning = true;
		MSG msg = { 0 };

		while (continueRunning)
		{
			// Peek for messages
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT)
				{
					continueRunning = false;
				}
			}

			// ... game logic ...
		}

		return (int)(msg.wParam);
	}
}