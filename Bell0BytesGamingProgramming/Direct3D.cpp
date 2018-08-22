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

#pragma endregion

namespace graphics
{
	Direct3D::Direct3D()
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

		util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("Direct3D was initialized successfully.");
	}


	Direct3D::~Direct3D()
	{
		util::ServiceLocator::GetFileLogger()->Print<util::SeverityType::info>("Direct3D was shut down successfully.");
	}
}