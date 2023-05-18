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
#include <irrKlang/irrKlang.h>

#include "Utils.h"
#include "Subwindow.h"

using namespace std;
using namespace irrklang;

class Settings : public Subwindow
{
private:
	int wWidth = 450, wHeight = 330;
	ISoundEngine* engine;
	const char* soundfile = "sound\\snd_txtral_ch1.wav";
	Utils::Font* hFontSmall;

	vector<shared_ptr<Toggle>> togs;
	shared_ptr<Button> confirm;
	shared_ptr<Button> cancel;

	Substack substack;
public:
	Settings() : Subwindow()
	{
		drawingRect = { (Width - wWidth) / 2, (Height - wHeight) / 2, wWidth, wHeight };
		engine = createIrrKlangDevice();
		hFontSmall = new Utils::Font(30, "8bitoperator JVE");

		for (int i = 0; i < 5; i++)
		{
			shared_ptr<Toggle> t = make_shared<Toggle>(100, hFontSmall);
			t->SetPos(drawingRect.X + 250, drawingRect.Y + 80 + 40 * i);
			togs.push_back(t);
			substack.Add(t);
		}

		confirm = make_shared<Button>(100, hFontSmall, "Confirm");
		confirm->SetPos(drawingRect.X, drawingRect.Y + 300);
		cancel = make_shared<Button>(100, hFontSmall, "Cancel");
		cancel->SetPos(drawingRect.X + 200, drawingRect.Y + 300);

		substack.AddMultiple({ confirm, cancel });
	}

	void Update(double dt)
	{
		substack.Update(dt);
	}

	bool OnConfirmChoiceEvent(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		for (auto& t : togs)
		{
			if (t->IsCursorInSubwindowRect())
			{
				t->OnConfirmChoiceEvent(hWnd, Message, wParam, lParam);
				return true;
			}
		}
		return true;
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

		// Draw dialog box
		DrawFineRect(g, &brush, drawingRect);
		g->DrawRectangle(&pen, drawingRect);


		Font f(hdcMem, hFont);
		StringFormat titleFormat;
		titleFormat.SetAlignment(StringAlignmentCenter);
		StringFormat optionFormat;


		CString s = "Ralsei's SETTINGS";
		PointF drawPt = PointF(Width / 2, drawingRect.Y + 20);
		g->DrawString(
			s, wcslen(s),
			&f, drawPt,
			&titleFormat,
			&textBrush
		);

		Font f2(hdcMem, hFontSmall->GetFont());
		CString s2[] =
		{
			"Efficiency Mode",
			"Efficiency Mode2",
			"Efficiency Mode3",
			"Efficiency Mode4",
			"Efficiency Mode5",
		};
		for (int i = 0; i < 5; i++)
		{
			PointF drawPt2 = PointF((Width - wWidth) / 2 + 20, drawingRect.Y + 80 + i * 40);
			g->DrawString(
				s2[i], wcslen(s2[i]),
				&f2, drawPt2,
				&optionFormat,
				&textBrush
			);
		}
		substack.Paint(g, hdcMem);

	}
};