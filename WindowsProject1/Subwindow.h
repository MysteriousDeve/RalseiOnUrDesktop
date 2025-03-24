#pragma once
#include "framework.h"

class Subwindow : public enable_shared_from_this<Subwindow>
{
public:
	typedef std::shared_ptr<Subwindow> SubwPtr;

protected:
	bool on = true;
	bool visible = true;
	std::vector<SubwPtr> children;
	SubwPtr owner;

	// Protected call for recursive update call
	void _Update(double dt)
	{
		Update(dt);
		for (auto ch = children.begin(); ch != children.end(); ch++)
		{
			if ((*ch)->IsOn()) (*ch)->_Update(dt);
		}
	}

	void _Paint(Graphics* g, HDC hdcMem)
	{
		Paint(g, hdcMem);
		for (auto ch = children.begin(); ch != children.end(); ch++)
		{
			if ((*ch)->IsOn() && (*ch)->IsVisible()) (*ch)->_Paint(g, hdcMem);
		}
	}

	bool _Evt(UINT Message, WPARAM wParam, LPARAM lParam)
	{
		bool isConsumed = InputEvent(Message, wParam, lParam);
		if (isConsumed) return true;

		for (auto ch = children.begin(); ch != children.end(); ch++)
		{
			isConsumed = (*ch)->_Evt(Message, wParam, lParam);
			if (isConsumed) return true;
		}
		return false;
	}

public:
	string name;
	Vector2 pos = { 0, 0 };
	Vector2 size = { 200, 200 };
	double zOrder = 0;
	Vector2 hook = { 0, 0 };
	Vector2 offset = { 0, 0 };

	Subwindow() {};
	~Subwindow();
	virtual void Ready() { };

	// CMS (Children Management System)
	void AddChild(SubwPtr child)
	{
		if (child != nullptr)
		{
			for (auto i = children.begin(); i != children.end(); i++)
			{
				if (*i == child) { abort(); return; }
			}
			child->owner = this->shared_from_this();
			children.push_back(child);
			child->Ready();
		}
		else abort();
	}

	void AddChildren(vector<SubwPtr> children)
	{
		for (SubwPtr c : children)
		{
			AddChild(c);
		}
	}
	void RemoveChild(SubwPtr child)
	{
		for (auto i = children.begin(); i != children.end(); i++)
		{
			if (*i == child)
			{
				// child->owner = nullptr;
				children.erase(i);
				break;
			}
		}
	}

	// Event call functions
	virtual void Update(double dt) {}
	virtual void Paint(Graphics* g, HDC hdcMem) {}
	virtual bool InputEvent(UINT Message, WPARAM wParam, LPARAM lParam);

	// On and visibility state
	void On();
	void Off();
	bool IsOn();
	void Hide();
	void Show();
	void SetVisible(bool visible);
	bool IsVisible();

	// Checker
	bool IsInSubwindowRect(Vector2 pt);
	bool IsCursorInSubwindowRect();

	// Getters
	Vector2 GetGlobalPosition();
	Vector2Int GetCursorPosition();
	Rect ToRect();

	SubwPtr GetOwner()
	{
		return owner;
	}
}; typedef Subwindow::SubwPtr SubwPtr;

#include "Platform.h"
