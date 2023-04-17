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
using namespace std;

int hMenuIndex = 100;
class RightClickMenu
{
private:
	vector<CString> options;
	bool on = false;
	HWND menuWnd;
	HINSTANCE inst;
public:
	POINT menuPos;

	RightClickMenu(vector<CString> options, HINSTANCE hInstance)
	{
		this->options = options;

		const wchar_t CLASS_NAME[] = L"Hello guys!";
		HCURSOR cursor[]
		{
			LoadCursor(0, IDC_ARROW),
			LoadCursor(0, IDC_HAND)
		};
		inst = hInstance;
		InitMenu();
	}

	~RightClickMenu()
	{
		DestroyWindow(menuWnd);
	}

	void InitMenu()
	{
		HWND hWnd = CreateWindowExW
		(
			WS_EX_TOPMOST | WS_EX_LAYERED,
			NULL,
			(LPCTSTR)NULL,
			WS_POPUP | WS_CHILD,
			0, 0, 1000, 1000,
			NULL,
			(HMENU)hMenuIndex,
			inst,
			NULL
		);
		hMenuIndex++;
	}

	void Update(double dt)
	{
		if (IsOn()) ShowWindow(menuWnd, SW_SHOW);
	}

	void SetMenuToMousePos()
	{
		GetCursorPos(&menuPos);
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

	BOOL processChildMsg(LPARAM lParam)
	{

	}
};
