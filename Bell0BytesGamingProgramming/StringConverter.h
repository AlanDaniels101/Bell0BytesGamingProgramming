#pragma once

#pragma region "Description"

/*******************************************************************************************************************************
* StringConverter.h : Converts between std::wstring and std::string
*
********************************************************************************************************************************/

#pragma endregion

#pragma region "Includes"

#include "stdafx.h"

#pragma endregion

namespace util {
	class StringConverter
	{
	public:
		static std::wstring s2ws(const std::string& str);
		static std::string ws2s(const std::wstring& ws);
	};
}


