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
		Direct3D(core::DirectXApp* directXApp);
		~Direct3D();

		util::Expected<void> Present();							// display the next backbuffer

		friend class core::DirectXApp;

	private:
		util::Expected<void> CreateResources();					// create device resources
		util::Expected<void> OnResize();						// resize resources

	private:
		Microsoft::WRL::ComPtr<ID3D11Device> dev;				// device
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> devCon;		// device context
		Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;		// swap chain

		DXGI_FORMAT desiredColoredFormat;

		core::DirectXApp* directXApp;
	};
}

