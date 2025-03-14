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
		Rect drawingRect = {};
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


class Button : public Subwindow
{

private:
	std::function<void()> postEvt = []() {};
	Utils::Font* newfont;
	CString label = "Button";

public:
	int width;
	Button(int width, Utils::Font* font, CString label) : Subwindow()
	{
		this->width = width;
		this->newfont = font;
		this->label = label;
	}

	Button(int width) : Subwindow()
	{
		this->width = width;
	}

	Button() : Subwindow() {}

	void SetPostEvt(std::function<void()> postEvt)
	{
		this->postEvt = postEvt;
	}

	void Update(double dt)
	{

	}

	bool OnConfirmChoiceEvent(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		postEvt();
		return true;
	}

	void SetPos(int x, int y)
	{
		pos.x = x;
		pos.y = y;
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
		Rect drawingRect = {};
		drawingRect.Width = width;
		drawingRect.Height = newfont->GetHeight() + 10;

		// Draw menu
		Font f(hdcMem, newfont->GetFont());
		StringFormat strformat;

		PointF drawPt = PointF(pos.x + 15, pos.y);
		g->DrawString(
			label, wcslen(label),
			&f, drawPt,
			&strformat,
			&(IsCursorInSubwindowRect() ? textBrushHightlighted : textBrush));
	}
};