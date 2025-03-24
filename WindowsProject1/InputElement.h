#pragma once
#include "Utils.h"
#include "Subwindow.h"
using namespace std;


enum InputFieldType
{
	__NOTYPE_ERROR__,
	TOGGLE,
	SPINBOX
};

class InputElement : public Subwindow
{
private:
	InputFieldType type;
	any value;
	any defaultValue;

protected:
	std::function<void(any)> postEvt = [](any b) {};
	Utils::Font* newfont; 

	void _AssignValue(any value)
	{
		this->value = value;
		postEvt(value);
	}
public:
	string title;

	InputElement(InputFieldType type, int width, Utils::Font* font, string title, any defaultValue) : Subwindow()
	{
		this->type = type;
		this->newfont = font;
		this->title = title;
		this->defaultValue = defaultValue;
		this->value = defaultValue;
		size.x = width;
		size.y = newfont->GetHeight();
	}

	void SetPostEvt(std::function<void(any)> postEvt)
	{
		this->postEvt = postEvt;
	}

	virtual bool InputEvent(UINT Message, WPARAM wParam, LPARAM lParam)
	{
		postEvt(value);
		return true;
	}

	void Reset()
	{
		value = defaultValue;
	}

	InputFieldType GetInputType()
	{
		return type;
	}

	any Get()
	{
		return any(value);
	}

	virtual void SetValue(any value)
	{
		_AssignValue(value);
	}

	void SetDefaultValue(any defaultValue)
	{
		this->defaultValue = defaultValue;
	}

	template <typename T>
	T GetValue()
	{
		return any_cast<T>(value);
	}
};

template <typename T, typename Param>
class InputElementBase : public InputElement
{
private:
	any param;
public:
	InputElementBase(InputFieldType type, int width, Utils::Font* font, string title, T defaultValue, Param param)
	: InputElement(type, width, font, title, any(defaultValue)) 
	{
		this->param = param;
	}

	void SetParam(any param)
	{
		this->param = param;
	}

	T GetValue()
	{
		return InputElement::GetValue<T>();
	}

	Param GetParam()
	{
		return any_cast<Param>(param);
	}

	Param CastParam(any param)
	{
		if (param.type() == typeid(Param))
		{
			return any_cast<Param>(param);
		}
		else return Param();
	}
};

class Toggle : public InputElementBase<bool, vector<string>>
{
public:
	Toggle(int width, Utils::Font* font, string title, any defaultValue = any(false), any param = any(vector<string>()))
	: InputElementBase<bool, vector<string>>(TOGGLE, width, font, title, any_cast<bool>(defaultValue), CastParam(param))
	{

	}

	bool InputEvent(UINT Message, WPARAM wParam, LPARAM lParam)
	{
		SetValue(!GetValue());
		postEvt(GetValue());
		return true;
	}

	void Paint(Graphics* g, HDC hdcMem) override
	{
		if (!IsOn()) return;

		// Draw init
		Gdiplus::SolidBrush textBrush(Color(255, 255, 255, 255));
		Gdiplus::SolidBrush textBrushHightlighted(Color(255, 255, 255, 0));

		// Draw menu
		Font f(hdcMem, newfont->GetFont());
		StringFormat strformat;

		strformat.SetAlignment(StringAlignmentNear);
		CString s = title.c_str();

		Vector2 globalPos = GetGlobalPosition();

		PointF drawPt = PointF(globalPos.x + 15, globalPos.y);
		g->DrawString(
			s, wcslen(s),
			&f, drawPt,
			&strformat,
			&(IsCursorInSubwindowRect() ? textBrushHightlighted : textBrush));

		if (GetParam().size() >= 2)
			s = GetParam()[GetValue()].c_str();
		else s = GetValue() ? "On" : "Off";

		strformat.SetAlignment(StringAlignmentFar);
		drawPt = PointF(globalPos.x + size.x - 15, globalPos.y);
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
	StringAlignment alignment;

public:
	Button(int width, Utils::Font* font, CString label) : Subwindow()
	{
		this->newfont = font;
		this->label = label;
		size.x = width;
		size.y = font->GetHeight();
	}

	Button(int width) : Subwindow()
	{
		size.x = width;
	}

	Button() : Subwindow() {}

	void SetPostEvt(std::function<void()> postEvt)
	{
		this->postEvt = postEvt;
	}

	bool InputEvent(UINT Message, WPARAM wParam, LPARAM lParam)
	{
		postEvt();
		return true;
	}

	void SetAlignment(StringAlignment alignment)
	{
		this->alignment = alignment;
	}

	void Paint(Graphics* g, HDC hdcMem) override
	{
		if (!IsOn()) return;

		// Draw init
		Gdiplus::SolidBrush textBrush(Color(255, 255, 255, 255));
		Gdiplus::SolidBrush textBrushHightlighted(Color(255, 255, 255, 0));

		// Draw menu
		Font f(hdcMem, newfont->GetFont());
		StringFormat strformat;
		strformat.SetAlignment(alignment);

		Vector2 globalPos = GetGlobalPosition();

		PointF drawPt = PointF(globalPos.x + size.x * ((int)alignment / 2.0), globalPos.y);
		g->DrawString(
			label, wcslen(label),
			&f, drawPt,
			&strformat,
			&(IsCursorInSubwindowRect() ? textBrushHightlighted : textBrush));
	}
};




class Spinbox : public InputElementBase<int, vector<int>>
{
private:
	int currentIndex = 0;

public:
	Spinbox(int width, Utils::Font* font, string title, any param)
	: InputElementBase<int, vector<int>>(SPINBOX, width, font, title, 0, any_cast<vector<int>>(param))
	{
		vector<int> permittedValues = GetParam();
		if (permittedValues.empty())
		{
			permittedValues.push_back(0);
		}
		SetDefaultValue(permittedValues[0]);
		Reset();
		SetParam(permittedValues);
	}

	void SetValue(any newValue) override
	{
		auto permittedValues = GetParam();

		int valueInt = any_cast<int>(newValue);
		int smallestDiff = 0;
		int closestIdx = 0;
		if (permittedValues.size() > 0)
		{
			smallestDiff = abs(valueInt - permittedValues[0]);
		}
		for (int i = 1; i < permittedValues.size(); i++)
		{
			int diff = abs(valueInt - permittedValues[i]);
			if (diff < smallestDiff)
			{
				smallestDiff = diff;
				closestIdx = i;
			}
		}
		_AssignValue(permittedValues[closestIdx]);
		currentIndex = closestIdx;
	}

	bool InputEvent(UINT Message, WPARAM wParam, LPARAM lParam)
	{
		Next();
		postEvt(GetValue());
		return true;
	}

	void Next()
	{
		currentIndex = (currentIndex + 1) % GetParam().size();
		_AssignValue(GetParam()[currentIndex]);
	}

	void Paint(Graphics* g, HDC hdcMem) override
	{
		if (!IsOn()) return;

		// Draw init
		Gdiplus::Pen pen(Color(255, 255, 255, 255));
		Gdiplus::SolidBrush textBrush(Color(255, 255, 255, 255));
		Gdiplus::SolidBrush textBrushHightlighted(Color(255, 255, 255, 0));

		// Draw menu
		Font f(hdcMem, newfont->GetFont());
		StringFormat strformat;

		// Get drawing pos
		Vector2 globalPos = GetGlobalPosition();

		// Title
		strformat.SetAlignment(StringAlignmentNear);
		CString s = title.c_str();
		PointF drawPt = PointF(globalPos.x + 15, globalPos.y);
		g->DrawString(
			s, wcslen(s),
			&f, drawPt,
			&strformat,
			&(IsCursorInSubwindowRect() ? textBrushHightlighted : textBrush));

		// Value
		strformat.SetAlignment(StringAlignmentFar);
		s = to_string(GetValue()).c_str();
		drawPt = PointF(globalPos.x + size.x - 15, globalPos.y);
		g->DrawString(
			s, wcslen(s),
			&f, drawPt,
			&strformat,
			&(IsCursorInSubwindowRect() ? textBrushHightlighted : textBrush));
	}
};