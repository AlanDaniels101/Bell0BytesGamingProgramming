#pragma region "Description"

/*******************************************************************************************************************************
* Bell0BytesGamingProgramming.h : Defines the entry point for the application.
*
* Create a swapchain (render/display buffers)
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
	void Update(double deltaTime);
	util::Expected<int> Render(double farSeer);

	// Enter the main event loop
	util::Expected<int> Run() override;
};