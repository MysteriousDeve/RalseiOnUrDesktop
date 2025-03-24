#include "Subwindow.h"


Subwindow::~Subwindow()
{
	auto owner = GetOwner();
	if (owner)
	{
		owner->RemoveChild(this->shared_from_this());
	}
}

bool Subwindow::InputEvent(UINT Message, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool Subwindow::IsInSubwindowRect(Vector2 pt)
{
	Rect rc = ToRect();
	return rc.X <= pt.x and rc.Y <= pt.y and rc.X + rc.Width > pt.x and rc.Y + rc.Height > pt.y;
}

bool Subwindow::IsCursorInSubwindowRect()
{
	POINT pt;
	GetCursorPos(&pt);
	return IsInSubwindowRect(pt);
}

Vector2Int Subwindow::GetCursorPosition()
{
	POINT pt;
	GetCursorPos(&pt);
	Vector2Int cursorPos = Vector2Int(pt);
	return cursorPos;
}

Rect Subwindow::ToRect()
{
	Vector2 posHooked = GetGlobalPosition();
	return Rect(posHooked.x, posHooked.y, size.x, size.y);
}

Vector2 Subwindow::GetGlobalPosition()
{
	Vector2 ownerCenter = { 0, 0 };
	Vector2 ownerSize = { 0, 0 };
	if (owner)
	{
		ownerCenter = owner->GetGlobalPosition();
		ownerSize = owner->size;
	}
	return Vector2(
		pos.x - size.x * offset.x + ownerCenter.x + ownerSize.x * hook.x,
		pos.y - size.y * offset.y + ownerCenter.y + ownerSize.y * hook.y
	);
}

// On off functions
void Subwindow::On()
{
	on = true;
}

void Subwindow::Off()
{
	on = false;
}

bool Subwindow::IsOn()
{
	return on;
}

void Subwindow::Hide()
{
	visible = false;
}

void Subwindow::Show()
{
	visible = true;
}

void Subwindow::SetVisible(bool visible)
{
	this->visible = visible;
}

bool Subwindow::IsVisible()
{
	return visible;
}
