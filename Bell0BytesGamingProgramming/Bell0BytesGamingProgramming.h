#pragma region "Description"

/*******************************************************************************************************************************
* Bell0BytesGamingProgramming.h : Defines the entry point for the application.
*
* Window application using lua config file
*
********************************************************************************************************************************/

#pragma endregion

#include "resource.h"

void StartLoggingService();

class DirectXGame : core::DirectXApp
{
public:
	DirectXGame(HINSTANCE hInstance);
	~DirectXGame();

	// Initialize and shutdown
	util::Expected<void> Init();
	void Shutdown(util::Expected<void>* expected = NULL);

	// Enter the main event loop
	util::Expected<int> Run();
};