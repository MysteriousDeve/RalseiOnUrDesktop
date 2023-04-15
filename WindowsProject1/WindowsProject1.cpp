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
#include "resource.h"

#pragma comment (lib, "uxtheme.lib")

#include "ImageLoader.h"
#pragma comment (lib,"Gdiplus.lib")
#pragma comment (lib,"msimg32.lib")




using namespace std;
using namespace Gdiplus;

int Width = 500;
int Height = 500;

int MinWidth = 100;
int MinHeight = 100;

int mouseIsDown = -1;
POINT wndMouseDragOffset;

HBITMAP drawBuffer;

LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

HBRUSH bkgClr = CreateSolidBrush(RGB(16, 16, 16));
HBRUSH flybarClr = CreateSolidBrush(RGB(36, 36, 36));
RECT flybarRect;
int flybarSize = 30;

char mode = -1;

BITMAP bmp;
HFONT hFont;

Gdiplus::Image* img;
Gdiplus::PrivateFontCollection fontcollection;
HANDLE hMyFont = INVALID_HANDLE_VALUE;

GdiplusStartupInput startInput;
ULONG_PTR gdiToken;

Ralsei* ralsei;

void Paint(HWND hWnd);
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
{
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
            if (milisec >= 1000 / (long double)60.0)
            {
                timer = hrc::now();

                // Handle window dragging
                if (mouseIsDown == 1)
                {
                    POINT cPos;
                    GetCursorPos(&cPos);

                    ralsei->x = cPos.x + wndMouseDragOffset.x;
                    ralsei->y = cPos.y + wndMouseDragOffset.y;
                    ralsei->vely = 0;
                }
                ralsei->Update(1.0/60.0);
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
                20,
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
                DEFAULT_QUALITY,
                VARIABLE_PITCH,
                "8bitoperator"
            );
        }

        case WM_LBUTTONDOWN:
        {
            SetCapture(hWnd);

            if (mouseIsDown == -1)
            {
                mouseIsDown = 0;
                return 0;
            }
            RECT wndRect;
            GetWindowRect(hWnd, &wndRect);

            POINT cPos;
            GetCursorPos(&cPos);

            if (PtInRect(&wndRect, cPos))
            {
                wndMouseDragOffset.x = ralsei->x - cPos.x;
                wndMouseDragOffset.y = ralsei->y - cPos.y;
                mouseIsDown = 1;
            }
            return 0;
        }

        case WM_LBUTTONUP:
        {
            ReleaseCapture();
            ralsei->vely = -200;
            mouseIsDown = false;
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

    // Draw dialog box
    RectF digrect(ralsei->x - 250, ralsei->y - 400, 500, 150);
    pen.SetAlignment(PenAlignmentInset);
    pen2.SetAlignment(PenAlignmentInset);
    g.DrawRectangle(&pen2, digrect);
    g.DrawRectangle(&pen, digrect);    

    LOGFONT MyLogFont = { -80, 0, 0, 0, 0, FALSE, FALSE, FALSE, ANSI_CHARSET,
                       OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                       FF_DONTCARE, L"8bitoperator JVE" };
    Font f(hdcMem, &MyLogFont);
    StringFormat strformat;
    wchar_t buf[] = L"The quick brown fox jumps over the lazy dog!";
    g.DrawString(buf, wcslen(buf), &f,
        PointF(ralsei->x - 250, ralsei->y - 400), &strformat, &brush);


    // ...draw me?
    auto cl = ralsei->GetSprite();
    auto p = ralsei->GetRenderPosition();
    auto s = ralsei->GetRenderDimension();
    Rect rrect(p.x, p.y, s.x, s.y);
    g.DrawImage(cl, rrect);


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




