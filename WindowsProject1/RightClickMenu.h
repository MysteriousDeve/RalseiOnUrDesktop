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
	std::function<void(int)> postEvt = [](int i) {};
	int selectionIndex = -1;
public:
	int width;
	RightClickMenu(vector<CString> options, vector<std::function<void()>> eventCalls, int width = 200) : Subwindow()
	{
		this->options = options;
		this->eventCalls = eventCalls;
		this->width = width;

		hook = { 0, 1 };
	}

	void SetOptionName(int index, CString newOptionName)
	{
		options[index] = newOptionName;
	}

	void SetPostEvt(std::function<void(int)> postEvt)
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
		postEvt(i);
		return true;
	}

	int GetCurrentHoverChoice()
	{
		Vector2Int pt = GetCursorPosition();

		for (int i = 0; i < options.size(); i++)
		{
			Gdiplus::Rect r = { (int)pos.x + 5, (int)pos.y + 10 + 40 * i, (int)size.x - 10, 35 };
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
		pos = GetCursorPosition();
	}

	void SetMenuPos(int x, int y)
	{
		pos.x = x;
		pos.y = y;
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
			Vector2 hookedPos = GetPositionHooked();

            // Draw dialog box
			Rect drawingRect = {(int)pos.x, (int)pos.y, (int)size.x, (int)(40 * options.size() + 25) };
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
				Gdiplus::PointF drawPt = Gdiplus::PointF(hookedPos.x + 15, hookedPos.y + 10 + 40 * i);
				g->DrawString(
					s, wcslen(s), 
					&f, drawPt,
					&strformat,
					&(selectionIndex == i ? textBrushHightlighted : textBrush));
			}
        }
    }
};


