#pragma once
#include "framework.h"
#include <string>

class Platform
{
public:
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
    HRESULT AddThumbarButtons(HWND hwnd);

private:
    // Desktop window resources
    HMENU     m_hMenu;
    RECT      m_rc;
    HWND      m_hWnd;
};


static HINSTANCE m_hInstance;
static std::wstring m_windowClassName;
