#pragma region "Description"

/*******************************************************************************************************************************
* Bell0BytesGamingProgramming.cpp : Defines the entry point for the application.
*
* High resolution timer
* 
********************************************************************************************************************************/

#pragma endregion

#pragma region "Includes"

#include "stdafx.h"

#include <lua.hpp>

// Project includes
#include "Expected.h"						// Custom exceptions
#include "ServiceLocator.h"					// Global access to common services
#include "App.h"							// The main game class
#include "Bell0BytesGamingProgramming.h"

#pragma endregion

#pragma region "Static Libraries"

#pragma comment(lib, "liblua53.a")	// Lua

#pragma endregion

#pragma region "WinMain"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,					// handle to an instance of this application
                     _In_opt_ HINSTANCE hPrevInstance,			// NULL, no longer used
                     _In_ LPWSTR    lpCmdLine,					// command line args
                     _In_ int       nCmdShow)					// how the window is showed
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	// Create and initialize the game
	DirectXGame game(hInstance);
	util::Expected<void> gameInitialization = game.Init();

	if (gameInitialization.isValid())
	{
		// Run the game
		util::Expected<int> ret = game.Run();

		// Shutdown after the game has completed
		game.Shutdown(&(util::Expected<void>)ret);

		return ret.get();
	}
	else
	{
		// Error initializing
		game.Shutdown(&gameInitialization);

		return -1;
	}

	return 0;
}

#pragma endregion

#pragma region "DirectXGame"

DirectXGame::DirectXGame(HINSTANCE hInstance) : DirectXApp(hInstance) {}
DirectXGame::~DirectXGame() {}

util::Expected<void> DirectXGame::Init()
{
	// Initialize the core application
	util::Expected<void> applicationInitialization = DirectXApp::Init();
	if (!applicationInitialization.isValid())
	{
		return applicationInitialization;
	}

	// log and return success
	util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("Game initialization was successful.");
	return {};
}

void DirectXGame::Shutdown(util::Expected<void>* expected)
{
	if (expected && !expected->isValid())
	{
		// Shutdown reason is an exception
		try
		{
			// ... clean up ...

			expected->get(); // throw
		}
		catch (std::exception& e)
		{
			if (DirectXApp::m_isLoggerActive)
			{
				// create and print error message string
				std::stringstream errorMessage;
				errorMessage << "The game is shutting down with a critical error: " << e.what();
				util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::error>(std::stringstream(errorMessage.str()));
			}
			return;
		}
	}

	// Normal shutdown
	util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("The game was shut down successfully.");
}

util::Expected<int> DirectXGame::Run()
{
	// Run the core DirectX app
	return DirectXApp::Run();
}

void DirectXGame::OnResize()
{
	// Call base
	return DirectXApp::OnResize();
}

#pragma endregion