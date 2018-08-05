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

namespace core
{
	class DirectXApp
	{
	public:
		friend class Window;
	
	protected:
		HINSTANCE appInstance;
		Window* appWindow;

		// Private ctor/dtor can be accessed by Window friend
		DirectXApp(HINSTANCE hInstance);
		~DirectXApp();

		// Initialize and shutdown
		virtual util::Expected<void> Init();
		virtual void Shutdown(util::Expected<void>* expected = NULL);

		// Enter the main event loop
		util::Expected<int> Run();
	};
}
