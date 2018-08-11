#pragma once

#pragma region "Description"

/*******************************************************************************************************************************
* App.h : The main game class
*
********************************************************************************************************************************/

#pragma endregion

#pragma region "Includes"

#include "stdafx.h"

// Project includes
#include "Expected.h"		// Custom exceptions
#include "Window.h"			// Window class
#include "Timer.h"			// Timer

#pragma endregion

namespace core
{
	class DirectXApp
	{
	public:
		friend class Window;
	
	protected:
		// Private ctor/dtor can be accessed by Window friend
		DirectXApp(HINSTANCE hInstance);
		~DirectXApp();

		// Initialize and shutdown
		virtual util::Expected<void> Init();
		virtual void Shutdown(util::Expected<void>* expected = NULL);

		// Game loop
		virtual util::Expected<int> Run();				// enter the main event loop
		void Update(double deltaTime);			// update the game world

		// Resize handling
		virtual void OnResize();

		// Logging helpers
		bool GetPathToMyDocuments();
		void CreateLoggingService();
		bool CheckConfigurationFile();

		// Timer functions
		void CalculateFrameStatistics();		// compute fps / mspf

	protected:
		HINSTANCE m_appInstance;
		Window* m_appWindow;

		// Logger state
		bool m_isLoggerActive;

		// Folder paths
		std::wstring m_pathToMyDocuments;
		std::wstring m_pathToLogFiles;
		std::wstring m_pathToConfigurationFiles;
		bool m_hasValidConfigurationFile;

		// Game state
		bool m_isPaused;
		bool m_hasStarted;

		// Timer
		Timer* timer;
		int fps;					// frames per second
		double mspf;				// milliseconds per frame
	};
}
