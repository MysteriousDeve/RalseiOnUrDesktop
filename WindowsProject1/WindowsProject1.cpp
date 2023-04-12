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

    SetLayeredWindowAttributes(hWnd, 0 /* not affect bkg color */, 255, LWA_ALPHA);
    ShowWindow(hWnd, SW_SHOW);

    img = Gdiplus::Image::FromFile(L"deltarune-sprites\\ralsei\\spr_ralseib_idle(0).png");
    if (img->GetLastStatus() != Gdiplus::Ok)
    {
        MessageBoxA(NULL, "The hell?", "Image not loaded", MB_OK);
        exit(0);
    }

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
                InvalidateRect(hWnd, NULL, FALSE);
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

    case WM_MOUSEMOVE:
    {

    }

    case WM_ERASEBKGND:
        return (LRESULT)1; // Tell Windows we handle the background erase, to prevent flickering ig...

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        SelectObject(hdc, GetStockObject(DC_BRUSH));
        RECT wrect, textRect;
        GetWindowRect(hWnd, &wrect);

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


        Gdiplus::Graphics g(hdc);
        g.Clear(Gdiplus::Color(20, 0, 0, 0));
        g.SetInterpolationMode(InterpolationModeNearestNeighbor);
        g.SetPixelOffsetMode(PixelOffsetModeNone);
        Rect gdirect(0, 0, 200, 200);
        g.DrawImage(img, gdirect);


        EndPaint(hWnd, &ps);
    }
    return 0;

    case WM_DESTROY:
    {
        GdiplusShutdown(gdiToken);
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProc(hWnd, Message, wParam, lParam);
}