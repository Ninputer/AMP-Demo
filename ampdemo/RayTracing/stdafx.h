// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// Don't use min and max macros, we'll use std functions instead
#define NOMINMAX

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>

// Standrd library declarations
#include <algorithm>
#include <fstream>
#include <map>
#include <queue>
#include <string>
#include <vector>

// Windows Header Files:
#include <windows.h>

// Other Windows Headers
#include <D2d1.h>
#include <DWrite.h>
#include <PropKey.h>
#include <PropVarUtil.h>
#include <ShellApi.h>
#include <ShlObj.h>
#include <StructuredQuery.h>
#include <ThumbCache.h>
#include <UIAnimation.h>
#include <WinCodec.h>

// Commonly used headers
#include "ComPtr.h"
#include "SharedObject.h"
#include "ComHelpers.h"
#include "Window.h"
#include "WindowMessageHandler.h"
#include "WindowFactory.h"
#include "Direct2DUtility.h"
#include "AnimationUtility.h"

#ifndef HINST_THISCOMPONENT
extern "C" IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

// Use the correct version of the common control library based on the currently selected CPU architecture
// This is needed in order to use TaskDialog, since TaskDialog requires version 6.0 of Comctl32.dll
#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif