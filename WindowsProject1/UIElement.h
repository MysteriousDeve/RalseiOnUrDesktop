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
#include <gdiplusfont.h>
#include <gdipluspen.h>
#include <gdiplusbrush.h>
#include <uxtheme.h>
#include <atlstr.h>
#include <functional>

#include "Utils.h"
#include "Subwindow.h"
using namespace std;

class Toggle : public Subwindow
{
private:
	std::function<void()> postEvt = []() {};
	int value = 0;
	Utils::Font* newfont;
public:
	int width;
	Toggle(int width, Utils::Font* font) : Subwindow()
	{
		this->width = width;
		this->newfont = font;
	}

	Toggle(int width) : Subwindow()
	{
		this->width = width;
	}

	void SetPostEvt(std::function<void()> postEvt)
	{
		this->postEvt = postEvt;
	}

	void Update(double dt)
	{

	}

	bool OnConfirmChoiceEvent(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		value = (int)!value;
		postEvt();
		return true;
	}

	void SetPos(int x, int y)
	{
		pos.x = x;
		pos.y = y;
		drawingRect.X = x;
		drawingRect.Y = y;
	}

	void On()
	{
		on = true;
	}

	void Paint(Graphics* g, HDC hdcMem)
	{
		if (!IsOn()) return;

		// Draw init
		Gdiplus::SolidBrush textBrush(Color(255, 255, 255, 255));
		Gdiplus::SolidBrush textBrushHightlighted(Color(255, 255, 255, 0));

		// Draw dialog box
		drawingRect.Width = width;
		drawingRect.Height = newfont->GetHeight() + 10;

		// Draw menu
		Font f(hdcMem, newfont->GetFont());
		StringFormat strformat;

		CString s = value ? "On" : "Off";
		PointF drawPt = PointF(pos.x + 15, pos.y);
		g->DrawString(
			s, wcslen(s),
			&f, drawPt,
			&strformat,
			&(IsCursorInSubwindowRect() ? textBrushHightlighted : textBrush));
	}
};


