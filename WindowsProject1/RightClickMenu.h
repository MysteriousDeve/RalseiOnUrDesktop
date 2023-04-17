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

#include "Utils.h"
using namespace std;

const wchar_t CLASS_NAME2[] = L"Hello guys!";
class RightClickMenu
{
private:
	vector<CString> options;
	bool on = false;
	HWND menuWnd;
	HINSTANCE inst;
	Image* selectingImg;
public:
	POINT menuPos;
	Rect drawingRect = { 0, 0, 200, 40 * 6 + 30 };

	RightClickMenu(vector<CString> options)
	{
		this->options = options;

		HCURSOR cursor[]
		{
			LoadCursor(0, IDC_ARROW),
			LoadCursor(0, IDC_HAND)
		};
		selectingImg = Gdiplus::Image::FromFile(L"deltarune-sprites\\Sprites\\spr_heart(0).png");
	}

	void Update(double dt)
	{
		
	}

	void SetMenuToMousePos()
	{
		GetCursorPos(&menuPos);
		drawingRect.X = menuPos.x;
		drawingRect.Y = menuPos.y;
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

    void ShowErrorMsgBox()
    {
        string str = "ERROR CREATING WINDOW! error code: ";
        str += to_string(GetLastError());
        MessageBoxA(NULL, str.c_str(), "ERROR!", MB_OK);
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
			drawingRect.Width = 240;
			drawingRect.Height = 40 * options.size() + 25;
            DrawFineRect(g, &brush, drawingRect);
            g->DrawRectangle(&pen, drawingRect);

            // Draw menu
            Font f(hdcMem, hFont);
            StringFormat strformat;
			for (int i = 0; i < options.size(); i++)
			{
				CString s = options[i];
				PointF drawPt = PointF(menuPos.x + 15, menuPos.y + 10 + 40 * i);
				RectF heartRect = { drawPt + PointF{ 5, 5 }, { 30, 30 } };
				g->DrawImage(selectingImg, heartRect);
				g->DrawString(s, wcslen(s), &f, drawPt + PointF{ 40, 0 }, & strformat, & textBrush);
			}
        }
    }

	bool IsInMenuRect(POINT pt)
	{
		return drawingRect.Contains(Point(pt.x, pt.y));
	}
};


