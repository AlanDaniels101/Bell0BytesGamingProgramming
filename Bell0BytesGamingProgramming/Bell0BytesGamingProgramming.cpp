#pragma region "Description"

/*******************************************************************************************************************************
* Bell0BytesGamingProgramming.cpp : Defines the entry point for the application.
*
* Use DirectWrite and Direct2D to write the FPS into the backbuffer
* 
********************************************************************************************************************************/

#pragma endregion

#pragma region "Includes"

#include "stdafx.h"

// Project includes
#include "Expected.h"						// Custom exceptions
#include "ServiceLocator.h"					// Global access to common services
#include "App.h"							// The main game class
#include "Bell0BytesGamingProgramming.h"

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
		util::Expected<int> returnValue = game.Run();

		// Shutdown after the game has completed
		util::Expected<void> convertedReturnValue(returnValue);
		game.Shutdown(&convertedReturnValue);

		if (returnValue.isValid())
		{
			return returnValue.get();
		}
		else
		{
			return -1;
		}
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
			if (DirectXApp::FileLoggerIsActive())
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

util::Expected<int> DirectXGame::Update(double deltaTime)
{
	return 0;
}

util::Expected<int> DirectXGame::Render(double farSeer)
{
	// Clear back buffer and depth/stencil buffer
	direct3D->ClearBuffers();

	// ... render ...

	// Print FPS
	if (!direct2D->PrintFPS().isValid())
	{
		return std::runtime_error("Failed to print FPS information!");
	}

	// Present
	if (!direct3D->Present().isValid())
	{
		return std::runtime_error("Failed to present the scene!");
	}

	return 0;
}

#pragma endregion