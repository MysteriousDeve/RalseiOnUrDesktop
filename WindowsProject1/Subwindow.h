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
	bool on = true;
public:
	POINT pos = { 0, 0 };
	Rect drawingRect = { 0, 0, 200, 200 };
	double zOrder = 0;

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

	bool IsCursorInSubwindowRect()
	{
		POINT pt;
		GetCursorPos(&pt);
		return IsInSubwindowRect(pt);
	}
}; typedef std::shared_ptr<Subwindow> SubwPtr;


class Substack
{
private:
	vector<SubwPtr> subwindows;

public:

	void Paint(Graphics* g, HDC hdc)
	{
		vector<SubwPtr> sorted = subwindows;
		sort(sorted.begin(), sorted.end(), [](SubwPtr a, SubwPtr b) { return a->zOrder < b->zOrder; });

		for (SubwPtr s : subwindows)
		{
			s->Paint(g, hdc);
		}
	}

	void Update(double dt)
	{
		vector<SubwPtr> sorted = subwindows;
		sort(sorted.begin(), sorted.end(), [](SubwPtr a, SubwPtr b) { return a->zOrder < b->zOrder; });

		for (SubwPtr s : subwindows)
		{
			s->Update(dt);
		}
	}

	void Add(SubwPtr sub)
	{
		subwindows.push_back(sub);
	}

	void AddMultiple(vector<SubwPtr> subs)
	{
		subwindows.insert(subwindows.end(), subs.begin(), subs.end());
	}
};