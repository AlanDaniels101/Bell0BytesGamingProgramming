#pragma region "Description"

/*******************************************************************************************************************************
* Bell0BytesGamingProgramming.h : Defines the entry point for the application.
*
* Game loop based on fixed time-period updates + predictive rendering
*
********************************************************************************************************************************/

#pragma endregion

#include "resource.h"

class DirectXGame : core::DirectXApp
{
public:
	DirectXGame(HINSTANCE hInstance);
	~DirectXGame();

	// Initialize and shutdown
	util::Expected<void> Init() override;
	void Shutdown(util::Expected<void>* expected = NULL) override;
	void OnResize() override;

	// Enter the main event loop
	util::Expected<int> Run() override;
};