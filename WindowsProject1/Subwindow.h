#pragma once
#include <Windows.h>
#include <WinUser.h>
#include <shlwapi.h>
#include <algorithm>
#include <chrono>
#include <math.h>
#include <vector>

#include <ObjIdl.h>
#include <gdiplus.h>
#include <gdiplusheaders.h>
#include <gdipluspen.h>
#include <gdiplusbrush.h>
#include <uxtheme.h>
#include <atlstr.h>
#include <functional>

#include "Utils.h"
using namespace std;

class Subwindow
{
protected:
	bool on = false;
public:
	POINT pos;
	Rect drawingRect = { 0, 0, 200, 200 };

	Subwindow()
	{

	}

	void Update(double dt)
	{

	}

	bool OnConfirmChoiceEvent(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{

	}

	void On()
	{
		on = true;
	}

	void Off()
	{
		on = false;
	}

	bool IsOn()
	{
		return on;
	}

	void Paint(Graphics* g, HDC hdcMem)
	{
		if (!IsOn()) return;
	}

	bool IsInSubwindowRect(POINT pt)
	{
		return drawingRect.Contains(Point(pt.x, pt.y));
	}
};


