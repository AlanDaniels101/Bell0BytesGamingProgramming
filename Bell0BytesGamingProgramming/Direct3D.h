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

namespace graphics
{
	class Direct3D
	{
	public:
		Direct3D();
		~Direct3D();

	private:
		Microsoft::WRL::ComPtr<ID3D11Device> dev;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> devCon;
	};
}

