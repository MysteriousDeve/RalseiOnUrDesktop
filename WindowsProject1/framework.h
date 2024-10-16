// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <WinUser.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <ShObjIdl.h>
#include <shobjidl_core.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <algorithm>
#include <chrono>
#include <math.h>
#include <fstream>
#include <string.h>

// GDI includes
#include <ObjIdl.h>
#include <gdiplus.h>
#include <gdiplusheaders.h>
#include <gdipluspen.h>
#include <gdiplusbrush.h>
#include <uxtheme.h>
#include <atlstr.h>
#include <shellscalingapi.h>
#include <functional>

// DirectX
#include <direct.h>
#include <d2d1.h>

// Libraries
#pragma comment (lib,"shcore.lib")
#pragma comment (lib,"Gdiplus.lib")
#pragma comment (lib,"msimg32.lib")
#pragma comment (lib,"winmm.lib")
#pragma comment (lib, "irrKlang.lib")
#pragma comment (lib, "uxtheme.lib")
