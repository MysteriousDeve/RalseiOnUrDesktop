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

class RightClickMenu : public Subwindow
{
private:
	vector<CString> options;
	vector<std::function<void()>> eventCalls;
	std::function<void()> postEvt = []() {};
	int selectionIndex = -1;
public:
	int width;
	RightClickMenu(vector<CString> options, vector<std::function<void()>> eventCalls, int width = 200) : Subwindow()
	{
		this->options = options;
		this->eventCalls = eventCalls;
		this->width = width;
	}

	void SetPostEvt(std::function<void()> postEvt)
	{
		this->postEvt = postEvt;
	}

	void Update(double dt)
	{
		if (IsOn())
		{
			selectionIndex = GetCurrentHoverChoice();
		}
	}

	bool OnConfirmChoiceEvent(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		int i = GetCurrentHoverChoice();
		if (i < 0 || i >= eventCalls.size()) return false;
		eventCalls[i]();
		postEvt();
		return true;
	}

	int GetCurrentHoverChoice()
	{
		POINT pt;
		GetCursorPos(&pt);

		for (int i = 0; i < options.size(); i++)
		{
			Rect r = { pos.x + 5, pos.y + 10 + 40 * i, drawingRect.Width - 10, 35 };
			BOOL b = r.Contains(pt.x, pt.y);
			if (b)
			{
				return i;
			}
		}
		return -1;
	}

	void SetMenuToMousePos()
	{
		GetCursorPos(&pos);
		drawingRect.X = pos.x;
		drawingRect.Y = pos.y;
	}

	void SetMenuPos(int x, int y)
	{
		pos.x = x;
		pos.y = y;
		drawingRect.X = x;
		drawingRect.Y = y;
	}

	void On()
	{
		on = true;
		selectionIndex = -1;
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
		Gdiplus::SolidBrush textBrushHightlighted(Color(255, 255, 255, 0));
        Gdiplus::SolidBrush brush(Color(255, 0, 0, 0));

        pen.SetAlignment(PenAlignmentInset);
        pen2.SetAlignment(PenAlignmentInset);

        if (IsOn())
        {
            // Draw dialog box
			drawingRect.Width = width;
			drawingRect.Height = 40 * options.size() + 25;
            DrawFineRect(g, &brush, drawingRect);
            g->DrawRectangle(&pen, drawingRect);

            // Draw menu
            Font f(hdcMem, hFont);
            StringFormat strformat;
			for (int i = 0; i < options.size(); i++)
			{
				CString s = options[i];
				PointF drawPt = PointF(pos.x + 15, pos.y + 10 + 40 * i);
				g->DrawString(
					s, wcslen(s), 
					&f, drawPt,
					&strformat,
					&(selectionIndex == i ? textBrushHightlighted : textBrush));
			}
        }
    }
};


