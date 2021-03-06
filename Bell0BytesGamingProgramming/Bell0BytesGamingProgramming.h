#pragma region "Description"

/*******************************************************************************************************************************
* Bell0BytesGamingProgramming.h : Defines the entry point for the application.
*
* Finish the Bell0Bytes game programming tutorial! This adds fullscreen support and the ability to change resolutions.
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

	// Initialize graphics
	util::Expected<void> InitGraphics();

	// Enter the main event loop
	util::Expected<int> Run() override;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	std::vector<graphics::Vertex> starField;
	D3D11_MAPPED_SUBRESOURCE mappedStarField;
};