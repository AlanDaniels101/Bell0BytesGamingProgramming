#pragma region "Description"

/*******************************************************************************************************************************
* Bell0BytesGamingProgramming.h : Defines the entry point for the application.
*
* Use DirectWrite and Direct2D to write the FPS into the backbuffer
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
	util::Expected<int> Update(double deltaTime);
	util::Expected<int> Render(double farSeer);

	// Enter the main event loop
	util::Expected<int> Run() override;
};