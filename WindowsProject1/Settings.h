#pragma once
#include "framework.h" 
#include <irrKlang/irrKlang.h>

#include "Utils.h"
#include "Subwindow.h"
#include "InputElement.h"

using namespace std;
using namespace irrklang;


class SettingField : public tuple<string, InputFieldType, function<void(any)>, any, any>
{
public:
	SettingField(string title, InputFieldType type, function<void(any)> callback, any defaultValue, any param = any())
	: tuple<string, InputFieldType, function<void(any)>, any, any>(title, type, callback, defaultValue, param)
	{}

	string getTitle() { return get<0>(*this); }
	InputFieldType getSettingType() { return get<1>(*this); }
	function<void(any)> getCallback() { return get<2>(*this); }
	any getDefaultValue() { return get<3>(*this); }
	any getParam() { return get<4>(*this); }
};

class Settings : public Subwindow
{
private:
	ISoundEngine* engine = nullptr;
	const char* soundfile = "sound\\snd_txtral_ch1.wav";
	Utils::Font* hFontSmall = nullptr;

	vector<shared_ptr<InputElement>> elements;
	vector<SettingField> settingFields;
	shared_ptr<Button> confirm;
	shared_ptr<Button> cancel;
public:
	Settings() : Subwindow() {}

	Settings(vector<SettingField> settings) : Subwindow()
	{
		this->settingFields = settings;
	}

	void Ready()
	{
		offset = { 0.5, 0.5 };
		hook = { 0.5, 0.5 };
		size = { 500, 500 };

		engine = createIrrKlangDevice();
		hFontSmall = new Utils::Font(30, "8bitoperator JVE");

		for (int i = 0; i < settingFields.size(); i++)
		{
			auto s = settingFields[i];
			shared_ptr<InputElement> t;
			if (s.getSettingType() == TOGGLE)
			{
				t = make_shared<Toggle>(400, hFontSmall, "Haha mayhay", s.getDefaultValue(), s.getParam());
			}
			else if (s.getSettingType() == SPINBOX)
			{
				t = make_shared<Spinbox>(400, hFontSmall, "Haha mayhay", s.getParam());
			}
			else { continue; }

			t->pos = { 0.0, 100.0 + 40 * i };
			t->offset = { 0.5, 0 };
			t->hook = { 0.5, 0 };
			t->title = s.getTitle();
			t->SetPostEvt(s.getCallback());
			t->On();
			elements.push_back(t);
			AddChild(t);
		}

		confirm = make_shared<Button>(100, hFontSmall, "Reset");
		confirm->pos = { -100, -30 };
		confirm->offset = { 0.5, 1 };
		confirm->hook = { 0.5, 1 };
		confirm->SetAlignment(StringAlignmentNear);
		confirm->On();
		confirm->SetPostEvt(
			[this]() { 
				for (int i = 0; i < elements.size(); i++) {
					elements[i]->Reset();
				}
			}
		);

		cancel = make_shared<Button>(100, hFontSmall, "Confirm");
		cancel->pos = { 100.0, -30.0 };
		cancel->offset = { 0.5, 1.0 };
		cancel->hook = { 0.5, 1.0 };
		cancel->SetAlignment(StringAlignmentFar);
		cancel->On();
		cancel->SetPostEvt(
			[this]() {
				Off();
			}
		);
		AddChildren({ confirm, cancel });

		LoadSettings();
		Off();
	}

	void LoadSettings()
	{
		string myLine;
		fstream fileHandler;
		fileHandler.open("config.txt");
		if (fileHandler.is_open())
		{
			int currentIdx = 0;
			while (currentIdx < elements.size() && getline(fileHandler, myLine))
			{
				if (elements[currentIdx]->GetInputType() == TOGGLE)
				{
					elements[currentIdx]->SetValue(myLine[0] == '1' ? true : false);
				}
				else if (elements[currentIdx]->GetInputType() == SPINBOX)
				{
					elements[currentIdx]->SetValue(stoi(&myLine[0]));
				}
				currentIdx++;
			}
		}
		fileHandler.close();
	}

	void SaveSettings()
	{
		fstream fileHandler;
		fileHandler.open("config.txt");
		fileHandler.clear();
		for (int i = 0; i < elements.size(); i++)
		{
			if (elements[i]->GetInputType() == TOGGLE)
			{
				fileHandler << (static_pointer_cast<Toggle>(elements[i])->GetValue() ? "1" : "0") << "\n";
			}
			else if (elements[i]->GetInputType() == SPINBOX)
			{
				fileHandler << to_string(static_pointer_cast<Spinbox>(elements[i])->GetValue()).c_str() << "\n";
			}
		}
		fileHandler.close();
	}


	bool InputEvent(UINT Message, WPARAM wParam, LPARAM lParam)
	{
		for (auto& t : elements)
		{
			if (t->IsCursorInSubwindowRect())
			{
				t->InputEvent(Message, wParam, lParam);
				SaveSettings();
				return true;
			}
		}

		if (confirm->IsCursorInSubwindowRect())
		{
			confirm->InputEvent(Message, wParam, lParam);
			return true;
		}

		if (cancel->IsCursorInSubwindowRect())
		{
			cancel->InputEvent(Message, wParam, lParam);
			return true;
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
		Rect drawingRect = ToRect();
		DrawFineRect(g, &brush, drawingRect);
		g->DrawRectangle(&pen, drawingRect);


		Font f(hdcMem, hFont);
		StringFormat titleFormat;
		titleFormat.SetAlignment(StringAlignmentCenter);
		StringFormat optionFormat;

		PointF posHooked = GetGlobalPosition();

		CString s = "Ralsei's SETTINGS";
		PointF drawPt = posHooked + PointF(size.x / 2, 20);
		g->DrawString(
			s, wcslen(s),
			&f, drawPt,
			&titleFormat,
			&textBrush
		);

		Font f2(hdcMem, hFontSmall->GetFont());
		for (auto ch = children.begin(); ch != children.end(); ch++)
		{
			(*ch)->Paint(g, hdcMem);
		}
	}
};