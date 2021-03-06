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
#include "Direct3D.h"
#include "Direct2D.h"
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
		dt(1000/(double)240),
		maxSkipFrames(10),
		m_hasStarted(false),
		showFPS(true),
		direct3D(NULL),
		direct2D(NULL)
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

		// Initialize Direct3D for graphics
		try
		{
			direct3D = new graphics::Direct3D(this);
		}
		catch (std::runtime_error)
		{
			return std::runtime_error("DirectXApp was unable to initialize Direct3D!");
		}

		// Initialize Direct2D
		try
		{
			direct2D = new graphics::Direct2D(this);
		}
		catch (std::runtime_error)
		{
			return std::runtime_error("DirectXApp was unable to initialize Direct2D!");
		}

		// log and return success
		m_hasStarted = true;
		util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("The DirectX application initialization was successful.");
		return {};
	}

	void DirectXApp::Shutdown(util::Expected<void>* expected)
	{
		if (direct2D)
		{
			delete direct2D;
		}

		if (direct3D)
		{
			delete direct3D;
		}

		if (m_appWindow)
		{
			delete m_appWindow;
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

	// Acquire user input
	void DirectXApp::OnKeyDown(WPARAM wParam, LPARAM lParam)
	{
		switch (wParam)
		{
		case VK_F1:
			// Toggle displaying FPS to screen
			showFPS = !showFPS;
			break;

		case VK_ESCAPE:
			PostMessage(m_appWindow->m_hWindow, WM_CLOSE, 0, 0);
			break;

		case VK_PRIOR:
			// Page up -> increase resolution
			direct3D->ChangeResolution(true);
			break;

		case VK_NEXT:
			// Page down -> decrease resolution
			direct3D->ChangeResolution(false);
			break;

		default:
			break;
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

		double accumulatedTime = 0.0;	// time accumulated by rendering rather than updating physics
		int nLoops = 0;					// number of completed loops while updating the game

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
				if (!CalculateFrameStatistics().isValid())
				{
					return util::Expected<int>("Critical error: Unable to calculate frame statistics!");
				}

				// ... get input ...

				util::Expected<int> result(0);

				// Add the rendering time (time between frames)
				accumulatedTime += timer->GetDeltaTime();

				// Update in fixed steps of delta-t until the accumulatedTime has been used up
				nLoops = 0;
				while (accumulatedTime >= dt && nLoops < maxSkipFrames)
				{
					result = Update(dt);
					if (!result.isValid())
					{
						return result;
					}
					accumulatedTime -= dt;
					nLoops++;
				}

				// Render, but predict the future by accounting for the remaining accumulatedTime
				result = Render(accumulatedTime / dt);
				if (!result.isValid())
				{
					return result;
				}
			}
			
		}
#ifndef NDEBUG
		util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("Leaving the game loop...");
#endif

		return (int)(msg.wParam);
	}

	util::Expected<void> DirectXApp::OnResize()
	{
#ifndef NDEBUG
		util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::warning>("The window was resized. The game graphics must be updated!");
#endif
		if (!direct3D->OnResize().isValid())
		{
			return std::runtime_error("Unable to resize Direct3D resources!");
		}

		return {};
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

	util::Expected<void> DirectXApp::CalculateFrameStatistics()
	{
		static int nFrames;				// number of frames seen
		static double elapsedTime;		// time since last call
		nFrames++;

		// Compute average statistics over one second
		if ((timer->GetTotalTime() - elapsedTime) >= 1.0)
		{
			fps = nFrames;
			mspf = 1000.0 / (double)fps;

			if (showFPS)
			{
				std::wostringstream outFPS;
				outFPS.precision(6);
				outFPS << "FPS: " << DirectXApp::fps << std::endl;
				outFPS << "mSPF: " << DirectXApp::mspf << std::endl;

				HRESULT hr = direct2D->writeFactory->CreateTextLayout(
					outFPS.str().c_str(),					// string
					(UINT32)outFPS.str().size(),			// string length
					direct2D->textFormatFPS.Get(),			// text format
					(float)m_appWindow->m_clientWidth,		// max width
					(float)m_appWindow->m_clientHeight,		// max height
					&direct2D->textLayoutFPS				// text layout
				);
				if (FAILED(hr))
				{
					return std::runtime_error("Critical error: Failed to create the text layout for FPS information!");
				}
			}

			// Reset
			nFrames = 0;
			elapsedTime += 1.0;
		}

		return {};
	}
}