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
#include "Direct3D.h"		// Graphics

#pragma endregion

namespace core
{
	class DirectXApp
	{
	public:
		friend class Window;
		friend class graphics::Direct3D;
#pragma region "Functions"
	protected:
		// Private ctor/dtor can be accessed by Window friend
		DirectXApp(HINSTANCE hInstance);
		~DirectXApp();

		// Initialize and shutdown
		virtual util::Expected<void> Init();
		virtual void Shutdown(util::Expected<void>* expected = NULL);

		// Acquire user input
		virtual void OnKeyDown(WPARAM wParam, LPARAM lParam);

		// Game loop
		virtual util::Expected<int> Run();			// enter the main event loop
		virtual void Update(double deltaTime) = 0;	// update the game world

		// Resize handling
		virtual util::Expected<void> OnResize();

		// Generate output
		virtual util::Expected<int> Render(double farseer) = 0;

		bool FileLoggerIsActive() { return m_isLoggerActive; }


	private:
		void CalculateFrameStatistics();		// compute fps / mspf

		// Logging helpers
		bool GetPathToMyDocuments();
		void CreateLoggingService();
		bool CheckConfigurationFile();
#pragma endregion
#pragma region "Variables"
	protected:
		HINSTANCE m_appInstance;
		Window* m_appWindow;
		graphics::Direct3D* direct3D;

		// Game state
		bool m_isPaused;

	private:
		// Folder paths
		std::wstring m_pathToMyDocuments;
		std::wstring m_pathToLogFiles;
		std::wstring m_pathToConfigurationFiles;

		bool m_hasValidConfigurationFile;

		// Logger state
		bool m_isLoggerActive;

		bool m_hasStarted;

		// Timer
		Timer* timer;
		int fps;					// frames per second
		double mspf;				// milliseconds per frame
		double dt;					// delta-t, the constant update rate of the game
		double maxSkipFrames;		// max number of frames to skip in the update loop
	
#pragma endregion
	};
}
