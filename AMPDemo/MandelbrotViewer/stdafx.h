// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// Don't use min and max macros, we'll use std functions instead
#define NOMINMAX

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