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
#include "Subwindow.h"
using namespace std;

class About : public Subwindow
{
private:

public:
	About() : Subwindow()
	{
		drawingRect = { 0, 0, 200, 200 };
	}

	void Update(double dt)
	{

	}

	void Paint(Graphics* g, HDC hdcMem)
	{
		if (!IsOn()) return;

		// Draw init
		Gdiplus::Pen pen(Color(255, 255, 255), 5);
		Gdiplus::Pen pen2(Color(255, 0, 0, 0), 500);
		Gdiplus::SolidBrush textBrush(Color(255, 255, 255, 255));
		Gdiplus::SolidBrush brush(Color(255, 0, 0, 0));

		pen.SetAlignment(PenAlignmentInset);
		pen2.SetAlignment(PenAlignmentInset);

		if (IsOn())
		{
			// Draw dialog box
			DrawFineRect(g, &brush, drawingRect);
			g->DrawRectangle(&pen, drawingRect);

			// Draw menu
			Font f(hdcMem, hFont);
			StringFormat strformat;

			CString s = "hello";
			PointF drawPt = PointF(pos.x + 15, pos.y + 10);
			g->DrawString(
				s, wcslen(s),
				&f, drawPt,
				&strformat,
				&textBrush
			);
		}
	}
};