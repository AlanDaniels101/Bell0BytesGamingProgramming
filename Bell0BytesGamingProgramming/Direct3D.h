#pragma once

#pragma region "Description"

/*******************************************************************************************************************************
* Direct3D.h
*
* Implement the Direct3D 11 interface
*
********************************************************************************************************************************/

#pragma endregion

#pragma region "Includes"

#include "stdafx.h"

// Direct3D
#include <d3d11.h>

// Project includes
#include "Expected.h"

#pragma endregion

#pragma region "Dynamically-Linked Libaries"

// Direct3D
#pragma comment(lib, "d3d11.lib")

#pragma endregion

// Forward declarations
class DirectXGame;
namespace core
{
	class DirectXApp;
	class Window;
}

namespace graphics
{
	// Represents a single vertex (point)
	struct Vertex
	{
		// position
		float x;
		float y;
		float z;

		// color
		float r;
		float g;
		float b;
	};

	struct ShaderBuffer
	{
		BYTE* buffer;
		int size;
	};

	class Direct3D
	{
	public:
		friend class core::DirectXApp;
		friend class Direct2D;
		friend class DirectXGame;
		friend class core::Window;

		Direct3D(core::DirectXApp* directXApp);
		~Direct3D();

	private:
		util::Expected<void> CreateResources();								// create device resources
		util::Expected<void> OnResize();									// resize resources
		util::Expected<ShaderBuffer> LoadShader(std::wstring filename);
		util::Expected<void> InitPipeline();								// initialize the graphics pipeline
		void ChangeResolution(bool increase);

		util::Expected<void> WriteCurrentModeDescriptionToConfigurationFile();
		util::Expected<void> ReadConfigurationFile();

		util::Expected<int> Present();							// display the next backbuffer
		void ClearBuffers();

	private:
		Microsoft::WRL::ComPtr<ID3D11Device> dev;							// device
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> devCon;					// device context
		Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;					// swap chain
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;	// render target
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;	// depth and stencil buffers

																			
		Microsoft::WRL::ComPtr<ID3D11VertexShader> standardVertexShader;	// the vertex shader
		Microsoft::WRL::ComPtr<ID3D11PixelShader> standardPixelShader;		// the pixel shader

		DXGI_FORMAT desiredColoredFormat;

		// Display modes
		unsigned int numberOfSupportedModes;
		DXGI_MODE_DESC* supportedModes;
		DXGI_MODE_DESC currentModeDescription;
		unsigned int currentModeIndex;
		bool startInFullscreen;
		BOOL currentlyInFullscreen;
		bool changeMode;

		core::DirectXApp* directXApp;
	};
}

