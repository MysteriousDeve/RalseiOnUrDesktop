#include <Windows.h>
#include <WinUser.h>
#include <shlwapi.h>
#include <algorithm>
#include <chrono>
#include <math.h>

#include <ObjIdl.h>
#include <gdiplus.h>
#include <gdiplusheaders.h>
#include <gdipluspen.h>
#include <gdiplusbrush.h>
#include <uxtheme.h>
#include <atlstr.h>
#include <shellscalingapi.h>
#include <functional>
#pragma comment (lib,"shcore.lib")

#include "TextPrinter.h"
#include "resource.h"
#pragma comment (lib, "uxtheme.lib")

#include "ImageLoader.h"
#include "RightClickMenu.h"
#include "Utils.h"
#include "About.h"
#pragma comment (lib,"Gdiplus.lib")
#pragma comment (lib,"msimg32.lib")
#pragma comment (lib,"winmm.lib")
#pragma comment (lib, "irrKlang.lib")


using namespace std;
using namespace Gdiplus;


int mouseIsDown = -1;
POINT wndMouseDragOffset;
POINT wndPosOld;

HBITMAP drawBuffer;

LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

HBRUSH bkgClr = CreateSolidBrush(RGB(16, 16, 16));
HBRUSH flybarClr = CreateSolidBrush(RGB(36, 36, 36));
RECT flybarRect;
int flybarSize = 30;
int verticalDPI = 96;

char mode = -1;

BITMAP bmp;

Gdiplus::Image* img;
Gdiplus::PrivateFontCollection fontcollection;

GdiplusStartupInput startInput;
ULONG_PTR gdiToken;

Ralsei* ralsei;
RightClickMenu* menu;
About* about;

constexpr long double delta = 1 / 60.0;

void Paint(HWND hWnd);
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
{
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

    Width = GetSystemMetrics(SM_CXSCREEN);
    Height = GetSystemMetrics(SM_CYSCREEN) - 1;

    GdiplusStartup(&gdiToken, &startInput, NULL);


    // Create windows
    const wchar_t CLASS_NAME[] = L"Hello guys!";
    HCURSOR cursor[]
    {
        LoadCursor(0, IDC_ARROW),
        LoadCursor(0, IDC_HAND)
    };
    WNDCLASSEX wClass = {};
    wClass.cbClsExtra = NULL;
    wClass.cbSize = sizeof(WNDCLASSEX);
    wClass.cbWndExtra = NULL;
    wClass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
    wClass.hCursor = cursor[0];
    wClass.hIcon = LoadIcon(0, IDI_APPLICATION);
    wClass.hInstance = hInstance;
    wClass.lpfnWndProc = WinProc;
    wClass.lpszClassName = CLASS_NAME;
    wClass.style = NULL;
    RegisterClassEx(&wClass);

    HWND hWnd = CreateWindowExW
    (
        WS_EX_TOPMOST | WS_EX_LAYERED,
        CLASS_NAME,
        L"Hello!",
        WS_POPUP,
        0, 0, Width, Height,
        NULL,
        NULL,
        hInstance,
        NULL
    ); 

    if (hWnd == NULL)
    {
        MessageBox(NULL, L"ERROR CREATING WINDOW!", L"ERROR!", MB_OK);
        return 0;
    }
    ShowWindow(hWnd, SW_SHOW);

    // UI definition
    ralsei = new Ralsei();
    menu = new RightClickMenu
    { 
        { L"Talk", L"Idle Mode", L"About", L"Exit" },
        { 
            []() {  },
            []() { ralsei->SetMode(ModeIdle); },
            [hWnd]() { about->On(); SetCapture(hWnd); },
            []() { exit(0); }
        },
    };
    about = new About();


    MSG msg = { };
    if (!SetForegroundWindow(hWnd)) MessageBox(NULL, L"Can't bring to front", L"", MB_OK);

    typedef std::chrono::high_resolution_clock hrc;
    static auto timer = hrc::now();
    while (TRUE)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            long double milisec = (hrc::now() - timer).count() / (long double)1000000.0;
            if (milisec >= delta * 1000)
            {
                timer = hrc::now();

                // Handle window dragging
                if (mouseIsDown == 1)
                {
                    POINT cPos;
                    GetCursorPos(&cPos);

                    ralsei->x = cPos.x + wndMouseDragOffset.x;
                    ralsei->y = cPos.y + wndMouseDragOffset.y;
                    ralsei->SetVelocity(0, 0);

                    wndPosOld = cPos;
                }
                ralsei->Update(delta);
                menu->Update(delta);
                about->Update(delta);

                Paint(hWnd);
            }
        }
    }
    return 0;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message)
    {
        case WM_INITDIALOG:
        {
            return 0;
        }

        case WM_CREATE:
        {
            hFont = CreateFontA(
                40,
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
            return 0;
        }

        case WM_LBUTTONDOWN:
        {
            POINT cPos;
            GetCursorPos(&cPos);

            if (menu->IsInSubwindowRect(cPos)) return 0;
            else menu->Off();

            SetCapture(hWnd);

            RECT wndRect;
            GetWindowRect(hWnd, &wndRect);

            if (PtInRect(&wndRect, cPos))
            {
                wndMouseDragOffset.x = ralsei->x - cPos.x;
                wndMouseDragOffset.y = ralsei->y - cPos.y;
                mouseIsDown = 1;
                wndPosOld = cPos;
                ralsei->isHolding = true;
            }
            return 0;
        }

        case WM_LBUTTONUP:
        {
            POINT cPos;
            GetCursorPos(&cPos);

            if (menu->IsOn())
            {
                menu->Off();
                if (menu->IsInSubwindowRect(cPos))
                {
                    if (menu->OnConfirmChoiceEvent(hWnd, Message, wParam, lParam)) return 0;
                }
            }
            else
            {
                if (about->IsOn())
                {
                    about->Off();
                }
                else
                {
                    ralsei->SetVelocity((cPos.x - wndPosOld.x) / delta, (cPos.y - wndPosOld.y) / delta);
                    ralsei->isHolding = false;
                }
            }
            ReleaseCapture();
            mouseIsDown = false;
            return 0;
        }

        case WM_RBUTTONDOWN:
        {
            menu->Off();
            if (!mouseIsDown) ReleaseCapture();
            return 0;
        }

        case WM_RBUTTONUP:
        {
            if (about->IsOn()) about->Off();
            menu->On();
            menu->SetMenuToMousePos();
            SetCapture(hWnd);
            return 0;
        }

        case WM_ERASEBKGND: return (LRESULT)1; // we handle background erase

        case WM_DESTROY:
        {
            delete ralsei; // Um... Kris...
            GdiplusShutdown(gdiToken);
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProc(hWnd, Message, wParam, lParam);
}


void Paint(HWND hWnd)
{
    RECT rc;
    HDC hdcMem;
    HBITMAP hbmMem;
    HBITMAP hbmOld;
    HDC hdc = GetDC(NULL);

    // Blend function
    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 255;
    bf.AlphaFormat = AC_SRC_ALPHA;

    // Get the size of the client rectangle.
    GetWindowRect(hWnd, &rc);

    // Create a compatible DC.
    hdcMem = CreateCompatibleDC(hdc);

    // Create a bitmap big enough for our client rectangle.
    hbmMem = CreateCompatibleBitmap(hdc,
        rc.right - rc.left,
        rc.bottom - rc.top);

    // Select the bitmap into the off-screen DC.
    hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

    // Bitmap
    BITMAP bm;
    GetObject(hbmMem, sizeof(bm), &bm);
    SIZE szBmp = { bm.bmWidth, bm.bmHeight };

    // Draw init
    Gdiplus::Graphics g(hdcMem);
    g.SetInterpolationMode(InterpolationModeNearestNeighbor);
    g.SetPixelOffsetMode(PixelOffsetModeHalf);
    Gdiplus::Pen pen(Color(255, 255, 255), 5);
    Gdiplus::Pen pen2(Color(255, 0, 0, 0), 500);
    Gdiplus::SolidBrush textBrush(Color(255, 255, 255, 255));
    Gdiplus::SolidBrush brush(Color(255, 0, 0, 0));

    pen.SetAlignment(PenAlignmentInset);
    pen2.SetAlignment(PenAlignmentInset);


    Font f(hdcMem, hFont);
    StringFormat strformat;
    if (ralsei->IsSpeaking())
    {
        // Draw dialog box
        RectF digrect(ralsei->x - 250, ralsei->y - 400, 500, 150);
        DrawFineRect(&g, &brush, digrect);
        g.DrawRectangle(&pen, digrect);

        // Draw text
        ATL::CString cstr = ralsei->GetCurrentSpeech();
        g.DrawString(cstr, wcslen(cstr), &f,
            PointF(ralsei->x - 250 + 45, ralsei->y - 400 + 10), &strformat, &textBrush);
        g.DrawString(L"*", wcslen(L"*"), &f,
            PointF(ralsei->x - 250 + 15, ralsei->y - 400 + 10), &strformat, &textBrush);
    }
    else if (true)
    {
        RectF digrect(ralsei->x - 250, ralsei->y - 520, 500, 270);
        DrawFineRect(&g, &brush, digrect);
        g.DrawRectangle(&pen, digrect);

        // Draw text
        ATL::CString cstr = "* About yourself\n* Dark World\n* Prophecy\n* Soul\n* I got homework\n* Nothing";
        g.DrawString(cstr, wcslen(cstr), &f,
            PointF(ralsei->x - 250 + 15, ralsei->y - 520 + 10), &strformat, &textBrush);
    }


    // ...draw me?
    auto cl = ralsei->GetSprite();
    auto p = ralsei->GetRenderPosition();
    auto s = ralsei->GetRenderDimension();
    Rect rrect(p.x, p.y, s.x, s.y);
    g.DrawImage(cl, rrect);

    menu->Paint(&g, hdcMem);
    about->Paint(&g, hdcMem);

    // Done with off-screen bitmap and DC.
    POINT ptSrc = { 0 };
    POINT ptDest = { rc.left, rc.top };
    BOOL bRet = UpdateLayeredWindow(
        hWnd,
        hdc,
        &ptDest,
        &szBmp,
        hdcMem,
        &ptSrc,
        0,
        &bf,
        ULW_ALPHA);
    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdc);
}


