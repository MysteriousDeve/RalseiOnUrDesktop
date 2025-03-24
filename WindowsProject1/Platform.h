#pragma once
#include "framework.h"
#include "Subwindow.h"
#include <map>

class Platform : public Subwindow
{
public:
    static Platform* topParent;

    Platform();
    ~Platform();

    HRESULT CreatePlatform(WNDPROC customProc = NULL);

    HWND GetWindowHandle() { return m_hWnd; };

    static LRESULT CALLBACK StaticWindowProc(
        HWND hWnd,
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam
    );

    HRESULT Run();
    HRESULT Render();
    HRESULT AddThumbarButtons(HWND hwnd);
    Vector2Int GetDimension();
    void TreeUpdate(double dt)
    {
        _Update(dt);
    }
    void TreePaint(Graphics* g, HDC hdcMem)
    {
        _Paint(g, hdcMem);
    }
    void TreeEvent(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
    {
        _Evt(Message, wParam, lParam);
    }

private:
    // Desktop window resources
    HMENU      m_hMenu;
    RECT       m_rc;
    Vector2Int m_windowSize;
    HWND       m_hWnd;
};


static HINSTANCE m_hInstance;
static std::wstring m_windowClassName;
