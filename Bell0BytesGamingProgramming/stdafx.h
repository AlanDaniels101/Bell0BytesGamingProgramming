// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

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