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
#pragma comment (lib,"shcore.lib")

#include "TextPrinter.h"
#include "resource.h"
#pragma comment (lib, "uxtheme.lib")

#include "ImageLoader.h"
#include "RightClickMenu.h"
#pragma comment (lib,"Gdiplus.lib")
#pragma comment (lib,"msimg32.lib")
#pragma comment (lib,"winmm.lib")
#pragma comment (lib, "irrKlang.lib")


using namespace std;
using namespace Gdiplus;

int Width = 500;
int Height = 500;

int MinWidth = 100;
int MinHeight = 100;

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
HFONT hFont;

Gdiplus::Image* img;
Gdiplus::PrivateFontCollection fontcollection;
HANDLE hMyFont = INVALID_HANDLE_VALUE;

GdiplusStartupInput startInput;
ULONG_PTR gdiToken;

Ralsei* ralsei;

constexpr long double delta = 1 / 60.0;

void Paint(HWND hWnd);
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
{
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

    Width = GetSystemMetrics(SM_CXSCREEN);
    Height = GetSystemMetrics(SM_CYSCREEN) - 1;

    GdiplusStartup(&gdiToken, &startInput, NULL);

    // Load font
    HMODULE hModule = GetModuleHandle(NULL);
    HRSRC res = FindResource(hModule, MAKEINTRESOURCE(IDR_FONT1), RT_FONT);
    if (res)
    {
        HGLOBAL mem = LoadResource(hModule, res);
        void* data = LockResource(mem);
        size_t len = SizeofResource(hModule, res);
        DWORD nFonts = 0;

        hMyFont = AddFontMemResourceEx(data, len, nullptr, &nFonts); // Fake install font!
    }
    else MessageBox(NULL, L"Resource (generic) not found", L"What the hell?!", MB_OK);



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

    ralsei = new Ralsei();

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
                Paint(hWnd);
                ShowWindow(hWnd, SW_SHOW);
            }
        }
    }
    return 0;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message)
    {
        case WM_CREATE:
        {
            RECT wndRect;
            GetWindowRect(hWnd, &wndRect);

            flybarRect = wndRect;
            wndRect.bottom = flybarSize;

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
        }

        case WM_LBUTTONDOWN:
        {
            rightClickMenu = false;
            if (mouseIsDown == -1)
            {
                mouseIsDown = 0;
                return 0;
            }
            SetCapture(hWnd);

            RECT wndRect;
            GetWindowRect(hWnd, &wndRect);

            POINT cPos;
            GetCursorPos(&cPos);
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
            rightClickMenu = false;
            ReleaseCapture();

            POINT cPos;
            GetCursorPos(&cPos);

            ralsei->SetVelocity((cPos.x - wndPosOld.x) / delta, (cPos.y - wndPosOld.y) / delta);
            ralsei->isHolding = false;

            mouseIsDown = false;
            return 0;
        }

        case WM_RBUTTONDOWN:
        {
            rightClickMenu = false;
            if (!mouseIsDown) ReleaseCapture();
            return 0;
        }

        case WM_RBUTTONUP:
        {
            rightClickMenu = true;
            GetCursorPos(&rightClickMenuPos);
            SetCapture(hWnd);
            return 0;
        }

        case WM_ERASEBKGND: return (LRESULT)1; // we handle background erase

        case WM_DESTROY:
        {
            delete ralsei; // Um... Kris...
            RemoveFontMemResourceEx(hMyFont);
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
    Gdiplus::Pen pen2(Color(255, 0, 0, 0), 250);
    Gdiplus::SolidBrush brush(Color(255, 255, 255, 255));
    pen.SetAlignment(PenAlignmentInset);
    pen2.SetAlignment(PenAlignmentInset);

    if (ralsei->IsSpeaking())
    {
        // Draw dialog box
        RectF digrect(ralsei->x - 250, ralsei->y - 400, 500, 150);
        g.DrawRectangle(&pen2, digrect);
        g.DrawRectangle(&pen, digrect);

        // Draw text
        Font f(hdcMem, hFont);
        StringFormat strformat;
        ATL::CString cstr = ralsei->GetCurrentSpeech();
        g.DrawString(cstr, wcslen(cstr), &f,
            PointF(ralsei->x - 250 + 45, ralsei->y - 400 + 10), &strformat, &brush);
        g.DrawString(L"*", wcslen(L"*"), &f,
            PointF(ralsei->x - 250 + 15, ralsei->y - 400 + 10), &strformat, &brush);
    }


    // ...draw me?
    auto cl = ralsei->GetSprite();
    auto p = ralsei->GetRenderPosition();
    auto s = ralsei->GetRenderDimension();
    Rect rrect(p.x, p.y, s.x, s.y);
    g.DrawImage(cl, rrect);


    if (rightClickMenu)
    {
        // Draw dialog box
        RectF digrect(rightClickMenuPos.x, rightClickMenuPos.y, 200, 40 * 6 + 30);
        g.DrawRectangle(&pen2, digrect);
        g.DrawRectangle(&pen, digrect);

        // Draw text
        Font f(hdcMem, hFont);
        StringFormat strformat;
        ATL::CString cstr = "Talk\nIdle Mode\nCommand\nDebug\nAbout\nExit";
        g.DrawString(cstr, wcslen(cstr), &f,
            PointF(rightClickMenuPos.x + 15, rightClickMenuPos.y + 10), &strformat, &brush);
    }

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




