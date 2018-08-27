#pragma once

#pragma region "Description"

/*******************************************************************************************************************************
* Direct2D.h
*
* Direct2D and DirectWrite for text and menus
*
********************************************************************************************************************************/

#pragma endregion

#pragma region "Includes"

#include "stdafx.h"

// DirectX
#include <d2d1_2.h>
#include <dwrite_2.h>

#include <shared_mutex> // shared pointers

// Project includes
#include "Expected.h"

#pragma endregion

#pragma region "Dynamically-Linked Libraries"

#pragma comment (lib, "d2d1.lib")
#pragma comment (lib, "dwrite.lib")

#pragma endregion

namespace core 
{
	class DirectXApp;
}

namespace graphics 
{
	class Direct2D
	{
	public:
		friend class core::DirectXApp;
		friend class Direct3D;
#pragma region "Functions"
	public:
		Direct2D(core::DirectXApp* directXApp);
		~Direct2D();

		util::Expected<void> PrintFPS();

	private:
		util::Expected<void> CreateDevice();					// creates the device and its context
		util::Expected<void> CreateBitmapRenderTarget();		// create the D2D render target (bitmap) - same as the Direct3D backbuffer
		util::Expected<void> InitializeTextFormats();			
#pragma endregion
#pragma region "Variables"
	public:
		// Brushes
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> yellowBrush;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> whiteBrush;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> blackBrush;

		// Text formats
		Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormatFPS;

		// Text layouts
		Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayoutFPS;

	private:
		core::DirectXApp* directXApp;

		Microsoft::WRL::ComPtr<IDWriteFactory2> writeFactory;	// DirectWrite factory
		Microsoft::WRL::ComPtr<ID2D1Factory2> factory;			// Direct2D factory
		Microsoft::WRL::ComPtr<ID2D1Device1> dev;				// Direct2D device
		Microsoft::WRL::ComPtr<ID2D1DeviceContext1> devCon;		// device context
	};
#pragma endregion
}

