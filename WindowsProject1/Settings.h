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
	int wWidth = 400, wHeight = 330;
	ISoundEngine* engine;
	const char* soundfile = "sound\\snd_txtral_ch1.wav";
	HFONT hFontSmall;

	Toggle* tog;
public:
	Settings() : Subwindow()
	{
		drawingRect = { (Width - wWidth) / 2, (Height - wHeight) / 2, wWidth, wHeight };
		engine = createIrrKlangDevice();
		hFontSmall = CreateFontA(
			30,
			0,
			0,
			0,
			FW_NORMAL,
			FALSE,
			FALSE,
			FALSE,
			DEFAULT_CHARSET,
			OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY,
			DEFAULT_PITCH,
			"8bitoperator JVE"
		);

		tog = new Toggle(100);
		tog->SetPos(drawingRect.X + 200, drawingRect.Y + 45);
	}

	void Update(double dt)
	{
		tog->Update(dt);
	}

	void On()
	{
		((Subwindow*)this)->On();
		if (!engine->isCurrentlyPlaying(soundfile)) engine->play2D(soundfile, true);
		tog->On();
	}

	void Off()
	{
		((Subwindow*)this)->Off();
		engine->stopAllSounds();
		tog->Off();
	}

	bool OnConfirmChoiceEvent(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if (tog->IsCursorInSubwindowRect())
		{
			tog->OnConfirmChoiceEvent(hWnd, Message, wParam, lParam);
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

		if (IsOn())
		{
			// Draw dialog box
			DrawFineRect(g, &brush, drawingRect);
			g->DrawRectangle(&pen, drawingRect);

			// Draw menu
			Font f(hdcMem, hFont);
			StringFormat strformat;
			strformat.SetAlignment(StringAlignmentCenter);

			CString s = "Ralsei's SETTINGS";
			PointF drawPt = PointF(Width / 2, drawingRect.Y + 20);
			g->DrawString(
				s, wcslen(s),
				&f, drawPt,
				&strformat,
				&textBrush
			);

			Font f2(hdcMem, hFontSmall);
			CString s2 = "lolololololololololol";
			PointF drawPt2 = PointF(Width / 2, drawingRect.Y + 80);
			g->DrawString(
				s2, wcslen(s2),
				&f2, drawPt2,
				&strformat,
				&textBrush
			);

			tog->Paint(g, hdcMem);
		}
	}
};