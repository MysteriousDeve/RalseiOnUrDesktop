#include "resource.h"
#include "framework.h"
#include "TextPrinter.h"

#include "ImageLoader.h"
#include "RightClickMenu.h"
#include "Utils.h"
#include "About.h"
#include "Settings.h"
#include "Platform.h"

#define R_EFFICIENCY L"--efficiency"

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
RightClickMenu* topicChoser;
About* about;
Settings* settings;

bool efficiencyMode = false;
bool forceEfficiencyMode = false;
bool mainLoop = true;

constexpr long double delta = 1 / 60.0;

void Paint(HWND hWnd);
void MainUpdate(HWND hWnd);
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
{
    HRESULT hr = S_OK;

    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    
    string myLine;
    fstream myFile_Handler;
    myFile_Handler.open("config.txt");
    if (myFile_Handler.is_open())
    {
        while (getline(myFile_Handler, myLine))
        {
            forceEfficiencyMode = (myLine[0] == '1' ? true : false);
        }
    }
    myFile_Handler.close();


    // start winapi process
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
    GdiplusStartup(&gdiToken, &startInput, NULL);


    // [Platform] Create a window.
    std::shared_ptr<Platform> winMain = std::shared_ptr<Platform>(new Platform());
    hr = winMain->CreatePlatform(WinProc);
    if (hr != S_OK)
    {
        return -999;
    }
    HWND hWnd = winMain->GetWindowHandle();

    // UI definition
    ralsei = new Ralsei();
    menu = new RightClickMenu
    { 
        { 
            L"Talk", 
            L"Idle Mode", 
            L"About",
            forceEfficiencyMode ? L"Toggle efficiency off" : L"Toggle efficiency on", 
            L"Settings",
            L"Exit"
        },
        { 
            []() { topicChoser->On(); },
            []() { ralsei->SetMode(ModeIdle); },
            [hWnd]() { if (!settings->IsOn()) { about->On(); SetCapture(hWnd); } },
            []() { forceEfficiencyMode = !forceEfficiencyMode; menu->SetOptionName(3, forceEfficiencyMode ? L"Toggle efficiency off" : L"Toggle efficiency on"); },
            []() { settings->On(); },
            []() { mainLoop = false; }
        },
        400
    };
    menu->SetPostEvt([](int i) { menu->Off(); ReleaseCapture(); });

    topicChoser = new RightClickMenu
    {
        { " * About yourself", " * Dark World", " * Prophecy", " * Soul", " * I got homework", " * Nothing"},
        {
            []() { convoIndex = 0; },
            []() { convoIndex = 1; },
            []() { convoIndex = 2; },
            []() { convoIndex = 3; },
            []() { convoIndex = 4; },
            []() { convoIndex = -1; },
        },
        500
    };
    topicChoser->SetPostEvt([](int i) { if (convoIndex != -1) ralsei->SetConvo(convo[convoIndex]); topicChoser->Off(); ReleaseCapture(); });
    about = new About();
    settings = new Settings();

    MSG msg = { };

    typedef std::chrono::high_resolution_clock hrc;
    static auto timer = hrc::now();
    SetTimer(hWnd, 0, delta * 1000, (TIMERPROC)WinProc);

    // Old main loop
    while (false && mainLoop)
    {
        efficiencyMode = forceEfficiencyMode || !isDeviceCharging();
        if (efficiencyMode)
        {
            if (!GetMessage(&msg, NULL, 0, 0)) break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
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
                MainUpdate(hWnd);
                DwmFlush();

                //long double milisec = (hrc::now() - timer).count() / (long double)1000000;
                //if (milisec >= delta * 1000)
                //{
                //    timer = hrc::now();
                //    MainUpdate(hWnd);
                //}
            }
        }
    }

    // New main loop
    winMain->Run();

    myFile_Handler.open("config.txt");
    myFile_Handler.clear();
    myFile_Handler << (forceEfficiencyMode ? "1" : "0");
    myFile_Handler.close();

    return 0;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message)
    {
        case WM_INITDIALOG:
        {
            break;
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
            break;
        }

        case WM_TIMER:
        {
            if (efficiencyMode) MainUpdate(hWnd);
            break;
        }

        case WM_LBUTTONDOWN:
        {
            POINT cPos;
            GetCursorPos(&cPos);

            if (menu->IsOn() && menu->IsInSubwindowRect(cPos)) return 0;
            else menu->Off();

            SetCapture(hWnd);

            RECT wndRect;
            GetWindowRect(hWnd, &wndRect);

            if (!ralsei->IsSpeaking())
            {
                if (PtInRect(&wndRect, cPos))
                {
                    wndMouseDragOffset.x = ralsei->x - cPos.x;
                    wndMouseDragOffset.y = ralsei->y - cPos.y;
                    mouseIsDown = 1;
                    wndPosOld = cPos;
                    ralsei->isHolding = true;

                    if (!topicChoser->IsInSubwindowRect(cPos) && topicChoser->IsOn()) topicChoser->Off();
                }
            }
            break;
        }

        case WM_LBUTTONUP:
        {
            POINT cPos;
            GetCursorPos(&cPos);

            if (menu->IsOn())
            {
                if (menu->IsInSubwindowRect(cPos))
                {
                    if (menu->OnConfirmChoiceEvent(hWnd, Message, wParam, lParam));
                }
            }
            else
            {
                if (settings->IsOn())
                {
                    if (settings->IsInSubwindowRect(cPos))
                    {
                        if (settings->OnConfirmChoiceEvent(hWnd, Message, wParam, lParam));
                    }
                }

                if (about->IsOn())
                {
                    about->Off();
                }
                else
                {
                    if (ralsei->isHolding)
                    {
                        ralsei->SetVelocity((cPos.x - wndPosOld.x) / delta, (cPos.y - wndPosOld.y) / delta);
                        ralsei->isHolding = false;
                    }

                    if (topicChoser->IsOn())
                    {
                        if (topicChoser->OnConfirmChoiceEvent(hWnd, Message, wParam, lParam))
                        {
                            topicChoser->IsInSubwindowRect(cPos);
                        }
                    }
                }
            }
            ReleaseCapture();
            mouseIsDown = false;
            break;
        }

        case WM_RBUTTONDOWN:
        {
            menu->Off();
            if (!mouseIsDown) ReleaseCapture();
            break;
        }

        case WM_RBUTTONUP:
        {
            if (about->IsOn()) about->Off();
            menu->On();
            menu->SetMenuToMousePos();
            SetCapture(hWnd);
            break;
        }

        case WM_KEYDOWN:
        {
            if (about->IsOn()) about->Off();
            if (menu->IsOn())
            {
                menu->Off();
                ReleaseCapture();
            }
            break;
        }

        case WM_KEYUP:
        {
            if (wParam == VK_APPS)
            {
                menu->On();
                menu->SetMenuToMousePos();
                SetCapture(hWnd);
            }
            break;
        }

        case WM_ERASEBKGND: return (LRESULT)1; // we handle background erase

        case WM_DESTROY:
        {
            int retCode = 0;
            delete ralsei; // Um... Kris...
            GdiplusShutdown(gdiToken);
            PostQuitMessage(retCode);
            return retCode;
        }
    }
    return DefWindowProc(hWnd, Message, wParam, lParam);
}

void MainUpdate(HWND hWnd)
{
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
    topicChoser->SetMenuPos(ralsei->x - topicChoser->width / 2, ralsei->y - 515);
    topicChoser->Update(delta);
    about->Update(delta);
    settings->Update(delta);

    Paint(hWnd);
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
    Gdiplus::SolidBrush textBrush(Color(255, 255, 255, 255));
    Gdiplus::SolidBrush brush(Color(255, 0, 0, 0));

    pen.SetAlignment(PenAlignmentInset);


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

    // ...draw me?
    auto cl = ralsei->GetSprite();
    auto p = ralsei->GetRenderPosition();
    auto s = ralsei->GetRenderDimension();
    Rect rrect(p.x, p.y, s.x, s.y);
    g.DrawImage(cl, rrect);

    topicChoser->Paint(&g, hdcMem);
    about->Paint(&g, hdcMem);
    menu->Paint(&g, hdcMem);
    settings->Paint(&g, hdcMem);

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



/*    typedef std::chrono::high_resolution_clock hrc;
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
            long double milisec = (hrc::now() - timer).count() / (long double)1000000;
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
                topicChoser->SetMenuPos(ralsei->x - topicChoser->width / 2, ralsei->y - 515);
                topicChoser->Update(delta);
                about->Update(delta);

                Paint(hWnd);
            }
        }

        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }








            if (efficiencyMode ? false : isDeviceCharging())
    {
        while (TRUE)
        {

        }
    }
    else
    {
        while (GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    }*/