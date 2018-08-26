#pragma region "Description"

/*******************************************************************************************************************************
* Window.cpp
*
* Implement the Direct3D 11 interface
*
********************************************************************************************************************************/

#pragma endregion

#pragma region "Includes"

#include "stdafx.h"

// Project includes
#include "ServiceLocator.h"					// Global access to common services
#include "Direct3D.h"
#include "App.h"

#pragma endregion

namespace graphics
{
	Direct3D::Direct3D(core::DirectXApp* directXApp) : 
		directXApp(directXApp), 
		desiredColoredFormat(DXGI_FORMAT_B8G8R8A8_UNORM)
	{
		HRESULT hr;

		// Define device creation flags
		// D3D11_CREATE_DEVICE_BGRA_SUPPORT - needed for Direct2D interoperability with Direct3D resources
		unsigned int createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifndef NDEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL featureLevel;	
		hr = D3D11CreateDevice(
			NULL,							// which GPU to use
			D3D_DRIVER_TYPE_HARDWARE,		// driver type
			0,								// handle to software rasterizer dll
			createDeviceFlags,				// flags
			NULL,							// feature levels
			0,								// number of feature levels
			D3D11_SDK_VERSION,				// SDK version
			&dev,							// D3D11Device
			&featureLevel,					// the selected feature level
			&devCon							// D3D11DeviceContext
		);

		if (FAILED(hr))
		{
			util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::error>("The creation of the Direct3D device and its context failed!");
			throw std::runtime_error("Unable to create the Direct3D device and its context!");
		}
		else if (featureLevel < D3D_FEATURE_LEVEL_11_0)
		{
			util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::error>("Critical error: DirectX 11 is not supported by your GPU!");
			throw std::runtime_error("Unable to create the Direct3D device and its context!");
		}

		// Create remaining resources
		if (!CreateResources().isValid())
		{
			util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::error>("Critical error: Creation of Direct3D resources failed!");
			throw std::runtime_error("Creation of Direct3D resources failed!");
		}

		util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("Direct3D was initialized successfully.");
	}


	Direct3D::~Direct3D()
	{
		util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("Direct3D was shut down successfully.");
	}

	util::Expected<void> Direct3D::CreateResources()
	{
		// Swap chain description
		DXGI_SWAP_CHAIN_DESC scd;
		scd.BufferDesc.Width = 0;
		scd.BufferDesc.Height = 0;
		scd.BufferDesc.RefreshRate.Numerator = 0;		// do not check refresh rate
		scd.BufferDesc.RefreshRate.Denominator = 1;
		scd.BufferDesc.Format = desiredColoredFormat;
		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		scd.SampleDesc.Count = 1;						// no MSAA
		scd.SampleDesc.Quality = 0;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = 3;
		scd.OutputWindow = directXApp->m_appWindow->GetMainWindowHandle();
		scd.Windowed = true;
		scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		// Get DXGI factory
		Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
		Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
		Microsoft::WRL::ComPtr<IDXGIFactory> dxgiFactory;

		HRESULT hr = dev.As(&dxgiDevice);
		if (FAILED(hr))
		{
			return std::runtime_error("The Direct3D device was unable to retrieve the underlying DXGI device!");
		}

		// Get the GPU that the device is running on
		hr = dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf());
		if (FAILED(hr))
		{
			return std::runtime_error("The DXGI Device was unable to get the GPU adapter!");
		}

		// Get the factory
		hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), &dxgiFactory);
		if (FAILED(hr))
		{
			return std::runtime_error("The DXGI Adapter was unable to get the factory!");
		}

		// Create the swap chain
		hr = dxgiFactory->CreateSwapChain(dev.Get(), &scd, swapChain.GetAddressOf());
		if (FAILED(hr))
		{
			return std::runtime_error("The creation of the swap chain failed!");
		}

		// Initial resize
		if (!OnResize().isValid())
		{
			return std::runtime_error("Direct3D was unable to resize its resources!");
		}

		return {};
	}

	util::Expected<void> Direct3D::OnResize()
	{
		devCon->ClearState();
		renderTargetView = nullptr;
		depthStencilView = nullptr;

		// Resize the swapchain
		HRESULT hr = swapChain->ResizeBuffers(0, 0, 0, desiredColoredFormat, 0);
		if (FAILED(hr))
		{
			return std::runtime_error("Direct3D was unable to resize the swap chain!");
		}

		// Get the swapchain backbuffer
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		unsigned int backBufferIndex = 0;
		hr = swapChain->GetBuffer(backBufferIndex, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		if (FAILED(hr))
		{
			return std::runtime_error("Direct3D was unable to acquire the back buffer!");
		}

		// Create new render target
		hr = dev->CreateRenderTargetView(backBuffer.Get(), 0, &renderTargetView);
		if (FAILED(hr))
		{
			return std::runtime_error("Direct3D was unable to create the render target view!");
		}

		// Depth and stencil buffer

		D3D11_TEXTURE2D_DESC dsd;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> dsBuffer;
		backBuffer->GetDesc(&dsd);
		dsd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsd.Usage = D3D11_USAGE_DEFAULT;				// gpu accessible only
		dsd.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		// Create buffer for depth/stencil
		hr = dev->CreateTexture2D(&dsd, NULL, dsBuffer.GetAddressOf());
		if (FAILED(hr))
		{
			return std::runtime_error("Direct3D was unable to create a 2D-texture!");
		}

		// Create depth/stencil
		hr = dev->CreateDepthStencilView(dsBuffer.Get(), NULL, depthStencilView.GetAddressOf());
		if (FAILED(hr))
		{
			return std::runtime_error("Direct3D was unable to create the depth and stencil buffer!");
		}

		// Activate depth and stencil buffers
		devCon->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

		// Set up viewport

		D3D11_VIEWPORT vp;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		vp.Width = (float)dsd.Width;
		vp.Height = (float)dsd.Height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;

		devCon->RSSetViewports(1, &vp);

		return {};
	}

	util::Expected<void> Direct3D::Present()
	{
		HRESULT hr = swapChain->Present(0, DXGI_PRESENT_DO_NOT_WAIT);
		if (FAILED(hr) && hr != DXGI_ERROR_WAS_STILL_DRAWING)
		{
			util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::error>("The presentation of the scene failed!");
			return std::runtime_error("Direct3D failed to present the scene!");
		}

		return {};
	}

	void Direct3D::ClearBuffers()
	{
		// Clear back buffer and depth/stencil buffer
		float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		devCon->ClearRenderTargetView(renderTargetView.Get(), black);
		devCon->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}