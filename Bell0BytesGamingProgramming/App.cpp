#pragma once

#pragma region "Description"

/*******************************************************************************************************************************
* App.cpp : The main game class
*
********************************************************************************************************************************/

#pragma endregion

#pragma region "Includes"

#include "stdafx.h"

// Windows
#include <Shlobj.h>
#include <Shlwapi.h>
#include <Pathcch.h>

// Project includes
#include "ServiceLocator.h"					// Global access to common services
#include "App.h"

#pragma endregion

#pragma region "Dynamically-Linked Libaries"

#pragma comment(lib, "ShLwApi.lib")
#pragma comment(lib, "Pathcch.lib")

#pragma endregion

namespace core
{
	DirectXApp::DirectXApp(HINSTANCE hInstance) :
		m_appInstance(hInstance),
		m_appWindow(NULL),
		m_isLoggerActive(false),
		m_hasValidConfigurationFile(false),
		m_isPaused(true),
		timer(NULL),
		fps(0),
		mspf(0.0),
		m_hasStarted(false)
	{
	}

	DirectXApp::~DirectXApp()
	{
		Shutdown();
	}

	util::Expected<void> DirectXApp::Init()
	{
		if (!GetPathToMyDocuments())
		{
			MessageBox(NULL, L"Unable to get the path to My Documents!", L"Critical Error!", MB_ICONEXCLAMATION | MB_OK);
			return std::runtime_error("Unable to retrieve the path to the My Documents folder!");
		}

		// Create logger
		try
		{
			CreateLoggingService();
		}
		catch (std::runtime_error)
		{
			MessageBox(NULL, L"Unable to start the logging service!", L"Critical Error!", MB_ICONEXCLAMATION | MB_OK);
			return std::runtime_error("Unable to start the logging service!");
		}

		// Check for valid config file
		if (!CheckConfigurationFile())
		{
			util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::warning>("Non-existent or invalid configuration file. Starting with default settings.");
		}

		// Create timer
		try
		{
			timer = new Timer();
		}
		catch (std::runtime_error)
		{
			return std::runtime_error("The high-precision timer could not be started!");
		}

		// Create the application window
		try
		{
			m_appWindow = new Window(this);
		}
		catch (std::runtime_error)
		{
			return std::runtime_error("DirectXApp was unable to create the main window!");
		}

		// log and return success
		m_hasStarted = true;
		util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("The DirectX application initialization was successful.");
		return {};
	}

	void DirectXApp::Shutdown(util::Expected<void>* expected)
	{
		if (m_appWindow)
		{
			delete m_appWindow;
		}

		if (m_appInstance)
		{
			m_appInstance = NULL;
		}

		if (timer)
		{
			delete timer;
		}
	
		if (m_isLoggerActive)
		{
			util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("The DirectX application was shutdown successfully.");
		}
	}

	// Main event loop
	util::Expected<int> DirectXApp::Run()
	{
#ifndef NDEBUG
		util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("Entering the game loop...");
#endif
		// Reset and start the timer
		timer->Reset();

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

			// Let the timer tick
			timer->Tick();

			if (!m_isPaused)
			{
				// Compute FPS
				CalculateFrameStatistics();

				// ... get input ...

				// Update based on elapsed time between frames
				Update(timer->GetDeltaTime());

				// ... generate output ...
			}
			
		}
#ifndef NDEBUG
		util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("Leaving the game loop...");
#endif

		return (int)(msg.wParam);
	}

	void DirectXApp::Update(double deltaTime)
	{

	}

	void DirectXApp::OnResize()
	{
#ifndef NDEBUG
		util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::warning>("The window was resized. The game graphics must be updated!");
#endif
	}

	bool DirectXApp::GetPathToMyDocuments()
	{
		PWSTR docPath = NULL;
		HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, NULL, NULL, &docPath);

#ifndef NDEBUG
		if (FAILED(hr))
		{
			return false;
		}
#endif

		m_pathToMyDocuments = docPath;

		::CoTaskMemFree(static_cast<void*>(docPath));

		// Log file path: "My Documents\Bell0BytesGameProgrammingGuide\logs"
		std::wstringstream pathToLogFiles;
		pathToLogFiles << m_pathToMyDocuments << L"\\Bell0BytesGameProgrammingGuide\\logs\\";
		m_pathToLogFiles = pathToLogFiles.str();

		std::wstringstream pathToConfigurationFiles;
		pathToConfigurationFiles << m_pathToMyDocuments << L"\\Bell0BytesGameProgrammingGuide\\config\\";
		m_pathToConfigurationFiles = pathToConfigurationFiles.str();

		return true;
	}

	void DirectXApp::CreateLoggingService()
	{
		// Create the file directory if it does not exist
		HRESULT hr;
		hr = SHCreateDirectory(NULL, m_pathToLogFiles.c_str());

#ifndef NDEBUG
		if (FAILED(hr))
		{
			throw std::runtime_error("Unable to create directory!");
		}
#endif

		std::wstring logFile = m_pathToLogFiles + L"\\logfile.log";

		// Create file logger
		std::shared_ptr<util::Logger<util::FileLogPolicy>> logger(new util::Logger<util::FileLogPolicy>(logFile));
		m_isLoggerActive = true;
		logger->SetThreadName("main");
		util::ServiceLocator::ProvideFileLoggingService(logger);

#ifndef NDEBUG
		// Print starting message
		util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("The file logger was created successfully.");
#endif
	}

	bool DirectXApp::CheckConfigurationFile()
	{
		// Create the file directory if it does not exist
		HRESULT hr;
		hr = SHCreateDirectory(NULL, m_pathToConfigurationFiles.c_str());

#ifndef NDEBUG
		if (FAILED(hr))
		{
			return false;
		}
#endif

		std::wstring pathToPrefsFile = m_pathToConfigurationFiles + L"\\prefs.lua";

		std::ifstream prefFile(pathToPrefsFile.c_str());
		if (prefFile.good())
		{
			// File exists
			if (prefFile.peek() == std::ifstream::traits_type::eof())
			{
				// File is empty
				try
				{
					util::Logger<util::FileLogPolicy> prefFileCreator(pathToPrefsFile.c_str());
					std::stringstream printPrefs;
					printPrefs << "config =\r\n{ \r\n\tresolution = { width = 800, height = 600 }\r\n}";
					prefFileCreator.Print<util::config>(printPrefs.str());
				}
				catch (std::runtime_error)
				{
					return false;
				}
			}
		}
		else
		{
			// File does not exist yet
			try
			{
				util::Logger<util::FileLogPolicy> prefFileCreator(pathToPrefsFile.c_str());
				std::stringstream printPrefs;
				printPrefs << "config =\r\n{ \r\n\tresolution = { width = 800, height = 600 }\r\n}";
				prefFileCreator.Print<util::config>(printPrefs.str());
			}
			catch (std::runtime_error)
			{
				return false;
			}
		}

		m_hasValidConfigurationFile = true;
		return true;
	}

	void DirectXApp::CalculateFrameStatistics()
	{
		static int nFrames;				// number of frames seen
		static double elapsedTime;		// time since last call
		nFrames++;

		// Compute average statistics over one second
		if ((timer->GetTotalTime() - elapsedTime) >= 1.0)
		{
			fps = nFrames;
			mspf = 1000.0 / (double)fps;

			// show statistics as window caption
			std::wstring windowCaption = L"bell0bytes engine --- fps: " + std::to_wstring(fps) + L" --- mspf: " + std::to_wstring(mspf);
			SetWindowText(m_appWindow->m_hWindow, windowCaption.c_str());

			// Reset
			nFrames = 0;
			elapsedTime += 1.0;
		}
	}
}