#pragma region "Description"

/*******************************************************************************************************************************
* Bell0BytesGamingProgramming.cpp : Defines the entry point for the application.
*
* Finish the Bell0Bytes game programming tutorial! This adds fullscreen support and the ability to change resolutions.
* 
********************************************************************************************************************************/

#pragma endregion

#pragma region "Includes"

#include "stdafx.h"

// Project includes
#include "Expected.h"						// Custom exceptions
#include "ServiceLocator.h"					// Global access to common services
#include "App.h"							// The main game class
#include "GraphicsHelper.h"					// Graphics helpers
#include "Bell0BytesGamingProgramming.h"

#pragma endregion

#pragma region "WinMain"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,					// handle to an instance of this application
                     _In_opt_ HINSTANCE hPrevInstance,			// NULL, no longer used
                     _In_ LPWSTR    lpCmdLine,					// command line args
                     _In_ int       nCmdShow)					// how the window is showed
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	// Create and initialize the game
	DirectXGame game(hInstance);
	util::Expected<void> gameInitialization = game.Init();

	if (gameInitialization.isValid())
	{
		// Run the game
		util::Expected<int> returnValue = game.Run();

		// Shutdown after the game has completed
		util::Expected<void> convertedReturnValue(returnValue);
		game.Shutdown(&convertedReturnValue);

		if (returnValue.isValid())
		{
			return returnValue.get();
		}
		else
		{
			return -1;
		}
	}
	else
	{
		// Error initializing
		game.Shutdown(&gameInitialization);

		return -1;
	}

	return 0;
}

#pragma endregion

#pragma region "DirectXGame"

DirectXGame::DirectXGame(HINSTANCE hInstance) : DirectXApp(hInstance) {}
DirectXGame::~DirectXGame() {}

util::Expected<void> DirectXGame::Init()
{
	// Initialize the core application
	util::Expected<void> applicationInitialization = DirectXApp::Init();
	if (!applicationInitialization.isValid())
	{
		return applicationInitialization;
	}

	// Initialize graphics
	applicationInitialization = InitGraphics();
	if (!applicationInitialization.isValid())
	{
		return applicationInitialization;
	}

	// log and return success
	util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("Game initialization was successful.");
	return {};
}

void DirectXGame::Shutdown(util::Expected<void>* expected)
{
	if (expected && !expected->isValid())
	{
		// Shutdown reason is an exception
		try
		{
			// ... clean up ...

			expected->get(); // throw
		}
		catch (std::exception& e)
		{
			if (DirectXApp::FileLoggerIsActive())
			{
				// create and print error message string
				std::stringstream errorMessage;
				errorMessage << "The game is shutting down with a critical error: " << e.what();
				util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::error>(std::stringstream(errorMessage.str()));
			}
			return;
		}
	}

	// Normal shutdown
	util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("The game was shut down successfully.");
}

util::Expected<void> DirectXGame::InitGraphics()
{
	// Create starfield
	const int numStars = 50000;
	for (int i = 0; i < numStars; i++)
	{
		starField.push_back(
			{
				graphics::randomPosition(), graphics::randomPosition(), graphics::randomPosition(),
				graphics::randomColor(), graphics::randomColor(), graphics::randomColor()
			}
		);
	}

	// Vertex buffer description
	D3D11_BUFFER_DESC bd;
	bd.ByteWidth = sizeof(graphics::Vertex) * static_cast<unsigned int>(starField.size());
	bd.Usage = D3D11_USAGE_DYNAMIC; // read by GPU write by CPU
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	// Create the subresource which is used to create a buffer from system memory
	D3D11_SUBRESOURCE_DATA srd = { starField.data(), 0, 0 };

	HRESULT hr = direct3D->dev->CreateBuffer(&bd, &srd, &vertexBuffer);
	if (FAILED(hr))
	{
		return "Critical Error: Unable to create vertex buffer!";
	}

	return {};
}

util::Expected<int> DirectXGame::Run()
{
	// Run the core DirectX app
	return DirectXApp::Run();
}

util::Expected<int> DirectXGame::Update(double deltaTime)
{
	// Update the vertex buffer
	HRESULT hr = direct3D->devCon->Map(
		vertexBuffer.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedStarField
	);
	if (FAILED(hr))
	{
		return "Critical error: Unable to map subresource!";
	}

	graphics::Vertex* v = reinterpret_cast<graphics::Vertex*>(mappedStarField.pData);

	auto begin = std::chrono::system_clock::now();

	for (int i = 0; i < starField.size(); i++)
	{
		v[i] = { graphics::randomPosition(), graphics::randomPosition(), graphics::randomPosition(),
				graphics::randomColor(), graphics::randomColor(), graphics::randomColor() };
	}

	direct3D->devCon->Unmap(vertexBuffer.Get(), 0);

	return 0;
}

util::Expected<int> DirectXGame::Render(double farSeer)
{
	// Clear back buffer and depth/stencil buffer
	direct3D->ClearBuffers();

	// Render

	// Print FPS
	if (!direct2D->PrintFPS().isValid())
	{
		return std::runtime_error("Failed to print FPS information!");
	}

	// Set the vertex buffer
	unsigned int stride = sizeof(graphics::Vertex);
	unsigned int offset = 0;
	direct3D->devCon->IASetVertexBuffers(
		0,
		1,
		vertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	// Set the primitive topology to be a list of triangles
	direct3D->devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw!
	direct3D->devCon->Draw(static_cast<unsigned int>(starField.size()) /*num vertices*/, 0 /*starting vertex index*/);

	// Present
	if (!direct3D->Present().isValid())
	{
		return std::runtime_error("Failed to present the scene!");
	}

	return 0;
}

#pragma endregion