#pragma once
#include "framework.h"
#include <irrKlang/irrKlang.h>

#include "Utils.h"
#include "Subwindow.h"

using namespace std;
using namespace irrklang;

class About : public Subwindow
{
private:
	ISoundEngine* engine;
	const char* soundfile = "sound\\castletown_empty.ogg";
	HFONT hFontSmall;
	shared_ptr<TextPrinter> prt;
public:
	About() : Subwindow()
	{
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

		offset = { 0.5, 0.5 };
		hook = { 0.5, 0.5 };
		size = { 400, 330 };
	}

	void Ready() override
	{
		prt = make_shared<TextPrinter>(
			"RalseiOnUrDesktop\nDeveloped by MysteriousDeve\nUse graphics and sounds\nfrom the game Deltarune\n(Have you felt fluffy yet?)\n\nClick anywhere to exit...",
			false, 0.28, "", INFINITY, false);
		AddChild(prt);
		Off();
	}

	void On()
	{
		on = true;
		if (!engine->isCurrentlyPlaying(soundfile)) engine->play2D(soundfile, true);
		prt->Reset();
		prt->isPlaying = true;
	}

	void Off()
	{
		on = false;
		engine->stopAllSounds();
		prt->isPlaying = false;
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
			Rect drawingRect = ToRect();
			DrawFineRect(g, &brush, drawingRect);
			g->DrawRectangle(&pen, drawingRect);

			// Draw menu
			Font f(hdcMem, hFont);
			StringFormat strformat;
			strformat.SetAlignment(StringAlignmentCenter);

			PointF hookedPos = (PointF)GetGlobalPosition();

			CString s = "ABOUT";
			PointF drawPt = hookedPos + PointF(size.x / 2, pos.y + 20);
			g->DrawString(
				s, wcslen(s),
				&f, drawPt,
				&strformat,
				&textBrush
			);

			Font f2(hdcMem, hFontSmall);
			CString s2 = prt->GetCurrent();
			PointF drawPt2 = hookedPos + PointF(size.x / 2, pos.y + 80);
			g->DrawString(
				s2, wcslen(s2),
				&f2, drawPt2,
				&strformat,
				&textBrush
			);
		}
	}
};