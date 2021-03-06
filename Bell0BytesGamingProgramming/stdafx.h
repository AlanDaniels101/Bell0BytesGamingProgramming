// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <wrl/client.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// C++ includes
#include <atomic>							// atomic objects (no data races) 
#include <thread>							// individual threats
#include <mutex>							// lockable objects
#include <iostream>							// input and output streams
#include <sstream>							// string streams
#include <fstream>							// file streams
#include <vector>							// vector containers
#include <map>								// map containers
#include <string>							// strings
#include <locale>							// immutable indexed set of immutable facets; each stream object of the C++ input/output library is associated with a std::locale object and uses its facets for parsing and formatting of all data
#include <codecvt>							// encapsulates conversion of character strings, including wide and multibyte, from one encoding to another 