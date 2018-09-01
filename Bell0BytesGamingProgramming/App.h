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
#include "Direct2D.h"

#pragma endregion

namespace core
{
	class DirectXApp
	{
	public:
		friend class Window;
		friend class graphics::Direct3D;
		friend class graphics::Direct2D;
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
		virtual util::Expected<int> Run();							// enter the main event loop
		virtual util::Expected<int> Update(double deltaTime) = 0;	// update the game world

		// Resize handling
		virtual util::Expected<void> OnResize();

		// Generate output
		virtual util::Expected<int> Render(double farseer) = 0;

		bool FileLoggerIsActive() { return m_isLoggerActive; }


	private:
		util::Expected<void> CalculateFrameStatistics();		// compute fps / mspf

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
		graphics::Direct2D* direct2D;

		// Game state
		bool m_isPaused;

		// Stats
		int fps;					// frames per second
		double mspf;				// milliseconds per frame

	private:
		// Folder paths
		std::wstring m_pathToMyDocuments;
		std::wstring m_pathToLogFiles;
		std::wstring m_pathToConfigurationFiles;
		std::wstring m_prefFile;						// the config file specifying resolution


		bool m_hasValidConfigurationFile;

		// Logger state
		bool m_isLoggerActive;

		bool m_hasStarted;			// app started
		bool showFPS;				// determines if FPS info should be printed to the screen

		// Timer
		Timer* timer;
		double dt;					// delta-t, the constant update rate of the game
		double maxSkipFrames;		// max number of frames to skip in the update loop
#pragma endregion
	};
}
