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

// Project includes
#include "App.h"
#include "ServiceLocator.h"
#include "Direct2D.h"

#pragma endregion

namespace graphics {

	Direct2D::Direct2D(core::DirectXApp* directXApp) :
		directXApp(directXApp)
	{
		// Create Direct2D device and device context
		if (!CreateDevice().isValid())
		{
			throw std::runtime_error("Crital error: Failed to initialize Direct2D!");
		}

		// Create Direct2D render target (bitmap)
		if (!CreateBitmapRenderTarget().isValid())
		{
			throw std::runtime_error("Critical error: Failed to create the bitmap render target for Direct2D!");
		}

		if (!InitializeTextFormats().isValid())
		{
			throw std::runtime_error("Critical error: Failed to create text formats!");
		}

		// Log success
		util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("Direct2D was successfully initialized.");
	}

	Direct2D::~Direct2D()
	{
		util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("Direct2D was shut down successfully.");
	}

	util::Expected<void> Direct2D::PrintFPS()
	{
		if (directXApp->showFPS && textLayoutFPS)
		{
			devCon->BeginDraw();

			devCon->DrawTextLayout(
				D2D1::Point2F(2.0f, 5.0f),	// origin / position of first letter
				textLayoutFPS.Get(),		// text layout
				yellowBrush.Get()			// brush
			);

			HRESULT hr = devCon->EndDraw();
			if (FAILED(hr))
			{
				return std::runtime_error("Critical error: Unable to draw FPS information!");
			}
		}

		return {};
	}

	util::Expected<void> Direct2D::CreateDevice()
	{
		// Create DirectWrite factory
		HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &writeFactory);
		if (FAILED(hr))
		{
			return std::runtime_error("Critical error: Unable to create the DirectWrite factory!");
		}

		// Create Direct2D factory
		D2D1_FACTORY_OPTIONS options;
#ifndef NDEBUG
		options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#else
		options.debugLevel = D2D1_DEBUG_LEVEL_NONE;
#endif

		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory2), &options, &factory);
		if (FAILED(hr))
		{
			return std::runtime_error("Critical error: Unable to create Direct2D Factory!");
		}

		// Get DXGI device
		Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
		hr = directXApp->direct3D->dev.Get()->QueryInterface(__uuidof(IDXGIDevice), &dxgiDevice);
		if (FAILED(hr))
		{
			return std::runtime_error("Critical error: Unable to get the DXGI device!");
		}

		// Create Direct2D device
		hr = factory->CreateDevice(dxgiDevice.Get(), &dev);
		if (FAILED(hr))
		{
			return std::runtime_error("Critical error: Unable to create the Direct2D device!");
		}

		// Create Direct2D device context
		hr = dev->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &devCon);
		if (FAILED(hr))
		{
			return std::runtime_error("Critical error: Unable to create the Direct2D device context!");
		}

		return {};
	}

	util::Expected<void> Direct2D::CreateBitmapRenderTarget()
	{
		// Set bitmap properties
		D2D1_BITMAP_PROPERTIES1 bp;
		bp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
		bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
		bp.dpiX = 96.0f;
		bp.dpiY = 96.0f;
		bp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
		bp.colorContext = nullptr;

		// Get Direct3D backbuffer
		Microsoft::WRL::ComPtr<IDXGISurface> dxgiBuffer;
		HRESULT hr = directXApp->direct3D->swapChain->GetBuffer(0, __uuidof(IDXGISurface), &dxgiBuffer);
		if (FAILED(hr))
		{
			return std::runtime_error("Critical error: Unable to retrieve the back buffer!");
		}

		// Create the bitmap
		Microsoft::WRL::ComPtr<ID2D1Bitmap1> targetBitmap;
		hr = devCon->CreateBitmapFromDxgiSurface(dxgiBuffer.Get(), &bp, &targetBitmap);
		if (FAILED(hr))
		{
			return std::runtime_error("Critical error: Unable to create the Direct2D bitmap from the DXGI surface!");
		}

		// Set this bitmap as the render target
		devCon->SetTarget(targetBitmap.Get());

		return {};
	}

	util::Expected<void> Direct2D::InitializeTextFormats()
	{
		// Create brushes
		HRESULT hr = devCon->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &yellowBrush);
		if (FAILED(hr))
		{
			return std::runtime_error("Critical error: Unable to create the yellow brush!");
		}
		hr = devCon->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &blackBrush);
		if (FAILED(hr))
		{
			return std::runtime_error("Critical error: Unable to create the black brush!");
		}
		hr = devCon->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &whiteBrush);
		if (FAILED(hr))
		{
			return std::runtime_error("Critical error: Unable to create the white brush!");
		}

		// Set text parameters

		// Text format
		hr = writeFactory.Get()->CreateTextFormat(
			L"Lucida Console",				// font family
			nullptr,						// font collection
			DWRITE_FONT_WEIGHT_LIGHT,		// font weight
			DWRITE_FONT_STYLE_NORMAL,		// font style
			DWRITE_FONT_STRETCH_NORMAL,		// font stretch
			12.0f,							// font size (in dp)
			L"en-GB",						// locale
			&textFormatFPS					// [out] text format object
		);
		if (FAILED(hr))
		{
			return std::runtime_error("Critical error: Unable to create text format for FPS information!");
		}

		// Text alignment
		hr = textFormatFPS->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		if (FAILED(hr))
		{
			return std::runtime_error("Critical error: Unable to set text alignment!");
		}

		// Paragraph alignment
		hr = textFormatFPS->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		if (FAILED(hr))
		{
			return std::runtime_error("Critical error: Unable to set paragraph alignment!");
		}

		return {};
	}
}