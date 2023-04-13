#include <Windows.h>
#include <WinUser.h>
#include <shlwapi.h>
#include <algorithm>
#include <chrono>
#include <math.h>

#include <ObjIdl.h>
#include <gdiplus.h>
#include <gdiplusheaders.h>
#include <uxtheme.h>
#pragma comment (lib, "uxtheme.lib")

#include "ImageLoader.cpp"
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

GdiplusStartupInput startInput;
ULONG_PTR gdiToken;

AnimationClip* clip;

void Paint(HWND hWnd);
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
{
    GdiplusStartup(&gdiToken, &startInput, NULL);
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
        L"Mello",
        WS_POPUP,
        20, 20, Width, Height,

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

    img = Gdiplus::Image::FromFile(L"deltarune-sprites\\ralsei\\spr_ralseib_idle(0).png");
    if (img->GetLastStatus() != Gdiplus::Ok)
    {
        MessageBoxA(NULL, "The hell?", "Image not loaded", MB_OK);
        exit(0);
    }
    clip = new AnimationClip(L"deltarune-sprites\\ralsei\\spr_ralseib_idle", 0.2, 5);
    

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

                    SetWindowPos(
                        hWnd, NULL,
                        cPos.x + wndMouseDragOffset.x,
                        cPos.y + wndMouseDragOffset.y,
                        -1, -1, SWP_NOSIZE
                    );
                }
                Paint(hWnd);
                ShowWindow(hWnd, SW_SHOW);
                // InvalidateRect(hWnd, NULL, FALSE);
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
                CLEARTYPE_QUALITY,
                VARIABLE_PITCH,
                "Segoe UI"
            );
        }

        case WM_LBUTTONDOWN:
        {
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
                wndMouseDragOffset.x = wndRect.left - cPos.x;
                wndMouseDragOffset.y = wndRect.top - cPos.y;
                mouseIsDown = 1;
            }
            return 0;
        }

        case WM_LBUTTONUP:
        {
            mouseIsDown = false;
            return 0;
        }

        case WM_ERASEBKGND: return (LRESULT)1; // Tell Windows we handle the background erase, to prevent flickering ig...

        case WM_DESTROY:
        {
            delete clip;
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

    // Draw image
    Gdiplus::Graphics g(hdcMem);
    g.SetInterpolationMode(InterpolationModeNearestNeighbor);
    g.SetPixelOffsetMode(PixelOffsetModeNone);
    Rect gdirect(0, 0, 200, 200);
    g.DrawImage(clip->Animate(1 / 60.0), gdirect);


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








    // Blt the changes to the screen DC.
    //BitBlt(lpPS->hdc,
    //    rc.left, rc.top,
    //    rc.right - rc.left, rc.bottom - rc.top,
    //    hdcMem,
    //    0, 0,
    //    SRCCOPY);

/*
 * Unused code
  
  

  void Paint(HWND hWnd, LPPAINTSTRUCT lpPS)
{
    RECT rc;
    HDC hdcMem;
    HBITMAP hbmMem;
    HGDIOBJ hbmOld;
    HBRUSH hbrBkGnd;
    HDC hdc = GetDC(NULL);

    // Blend function
    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 0xff;
    bf.AlphaFormat = AC_SRC_ALPHA;

    // Get the size of the client rectangle.
    GetClientRect(hWnd, &rc);

    // Create a compatible DC.
    hdcMem = CreateCompatibleDC(hdc);

    // Create a bitmap big enough for our client rectangle.
    hbmMem = CreateCompatibleBitmap(hdc,
        rc.right - rc.left,
        rc.bottom - rc.top);

    // Select the bitmap into the off-screen DC.
    hbmOld = SelectObject(hdcMem, hbmMem);

    // Bitmap
    BITMAP bm;
    GetObject(hbmMem, sizeof(bm), &bm);
    SIZE szBmp = { bm.bmWidth, bm.bmHeight };

    // Draw image
    Gdiplus::Graphics g(hdcMem);
    g.Clear(Gdiplus::Color(10, 0, 0, 0));
    g.SetInterpolationMode(InterpolationModeNearestNeighbor);
    g.SetPixelOffsetMode(PixelOffsetModeNone);
    Rect gdirect(0, 0, 200, 200);
    g.DrawImage(img, gdirect);


    // Done with off-screen bitmap and DC.
    // AlphaBlend(lpPS->hdc, 0, 0, rc.right, rc.bottom, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, bf);
    POINT ptSrc = { 0 };
    POINT ptDest = { rc.left, rc.top };
    BOOL bRet = UpdateLayeredWindow(
        hWnd,
        hdc,
        &ptDest,
        &szBmp,
        hdcMem,
        &ptSrc,
        RGB(255, 255, 255),
        &bf,
        ULW_COLORKEY);
    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
}

  
 
 

            FillRect(hdc, &ps.rcPaint, bkgClr);


            RECT flybarRect;
            flybarRect.right = wrect.right - wrect.left;
            flybarRect.bottom = flybarSize;
            SetRect(&textRect, 10, 0, wrect.right - 10, flybarSize);
            FillRect(hdc, &flybarRect, flybarClr);


            LPCWSTR msg = L"Hello";
            SelectObject(hdc, hFont);
            SetTextColor(hdc, RGB(255, 255, 255));
            SetBkMode(hdc, TRANSPARENT);
            DrawText(hdc, msg, -1, &textRect, DT_SINGLELINE | DT_MODIFYSTRING | DT_VCENTER);

*/