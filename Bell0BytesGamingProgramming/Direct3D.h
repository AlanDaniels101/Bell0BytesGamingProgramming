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

namespace core
{
	class DirectXApp;
}

namespace graphics
{
	class Direct3D
	{
	public:
		friend class core::DirectXApp;
		friend class Direct2D;

		Direct3D(core::DirectXApp* directXApp);
		~Direct3D();

		util::Expected<int> Present();							// display the next backbuffer
		void ClearBuffers();		

	private:
		util::Expected<void> CreateResources();					// create device resources
		util::Expected<void> OnResize();						// resize resources

	private:
		Microsoft::WRL::ComPtr<ID3D11Device> dev;							// device
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> devCon;					// device context
		Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;					// swap chain
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;	// render target
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;	// depth and stencil buffers

		DXGI_FORMAT desiredColoredFormat;

		core::DirectXApp* directXApp;
	};
}

