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

class RightClickMenu
{
private:
	vector<CString> options;
	bool on = false;
	POINT menuPos;
public:
	RightClickMenu(vector<CString> options)
	{
		this->options = options;

		const wchar_t CLASS_NAME[] = L"Hello guys!";
		HCURSOR cursor[]
		{
			LoadCursor(0, IDC_ARROW),
			LoadCursor(0, IDC_HAND)
		};

	}

	void SetMenuMousePos()
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

	operator bool()
	{
		return on;
	}

	HRESULT processMsg(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{

	}
};
