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

#pragma endregion

#define NUM_WINDOWS 5

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

		// Enter the main event loop
		util::Expected<int> Run();

		// Resize handling
		void OnResize();

		// Logging helpers
		bool GetPathToMyDocuments();
		void CreateLoggingService();
		bool CheckConfigurationFile();

	protected:
		HINSTANCE m_appInstance;
		Window* m_appWindow[NUM_WINDOWS];
		bool m_isLoggerActive;

		// Folder paths
		std::wstring m_pathToMyDocuments;
		std::wstring m_pathToLogFiles;
		std::wstring m_pathToConfigurationFiles;
		bool m_hasValidConfigurationFile;

		// Game state
		bool m_isPaused;
	};
}
