#include "resource.h"
#include "framework.h"
#include "TextPrinter.h"

#include "Ralsei.h"
#include "RightClickMenu.h"
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

shared_ptr<Platform> winMain;
shared_ptr<Ralsei> ralsei;
shared_ptr<RightClickMenu> menu;
shared_ptr<RightClickMenu> topicChoser;
shared_ptr<About> about;
shared_ptr<Settings> settings;

bool efficiencyMode = false;
bool mainLoop = true;

constexpr long double delta = 1 / 60.0;
double dt;
int framerate = 60;
long long frameCount = 0;

void Paint(HWND hWnd);
void MainUpdate(HWND hWnd, double delta);
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
{
    HRESULT hr = S_OK;

    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    // start winapi process
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
    GdiplusStartup(&gdiToken, &startInput, NULL);


    // [Platform] Create a window.
    winMain = std::make_shared<Platform>();
    hr = winMain->CreatePlatform(WinProc);
    if (hr != S_OK)
    {
        return -999;
    }
    HWND hWnd = winMain->GetWindowHandle();

    // UI definition
    ralsei = make_shared<Ralsei>();
    menu = make_shared<RightClickMenu>(RightClickMenu
    { 
        { 
            L"Talk", 
            L"Idle Mode",
            L"About",
            L"Settings",
            L"Reset Position",
            L"Exit",
        },
        { 
            []() { topicChoser->On(); },
            []() { ralsei->SetMode(ModeIdle); },
            [hWnd]() { if (!settings->IsOn()) { about->On(); SetCapture(hWnd); } },
            []() { settings->On(); },
            []() { ralsei->pos = { 300, 800 }; },
            []() { mainLoop = false; }
        },
        400
    });
    menu->SetPostEvt(
        [](int i) { 
            menu->Off(); 
            ReleaseCapture(); 
        }
    );

    topicChoser = make_shared<RightClickMenu>(RightClickMenu
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
    });
    topicChoser->SetPostEvt(
        [](int i) { 
            if (convoIndex != -1) ralsei->SetConvo(convo[convoIndex]); 
            topicChoser->Off();
            ReleaseCapture(); 
        }
    );
    
    about = make_shared<About>();

    vector<SettingField> settingFields =
    {
        { "Efficiency mode", TOGGLE, [](any value) { efficiencyMode = any_cast<bool>(value); }, false },
        { "Ralsei Mode", TOGGLE, [](any value) { ralsei->SetRemoveHat(any_cast<bool>(value)); }, true, vector<string>{ "Ch1", "Ch2" }},
        { "Skip intro", TOGGLE, [](any value) { if (!ralsei->hasSaidHi) ralsei->hasSaidHi = any_cast<bool>(value); }, false },
        { "Framerate", SPINBOX, [](any value) { framerate = any_cast<int>(value); }, 30, vector<int>{ 30, 45, 60 } },
    };
    settings = make_shared<Settings>(settingFields);
    MSG msg = { };

    // Setup
    winMain->Ready();
    winMain->AddChild(ralsei);
    winMain->AddChild(menu);
    winMain->AddChild(topicChoser);
    winMain->AddChild(about);
    winMain->AddChild(settings);
    // winMain->Run();

    typedef std::chrono::high_resolution_clock hrc;
    static auto timer = hrc::now();

    // Old main loop
    while (true && mainLoop)
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
            MainUpdate(hWnd, delta * (60.0 / (efficiencyMode ? 24 : framerate)));
            frameCount++;

            if (efficiencyMode)
            {
                if (frameCount % 2 == 0)
                {
                    DwmFlush();
                    DwmFlush();
                }
                else
                {
                    DwmFlush();
                    DwmFlush();
                    DwmFlush();
                }
            }
            else if (framerate == 30)
            {
                DwmFlush();
                DwmFlush();
            }
            else if (framerate == 45 && frameCount % 3 == 0)
            {
                DwmFlush();
                DwmFlush();
            }
            else
            {
                DwmFlush();
            }


            //long double milisec = (hrc::now() - timer).count() / (long double)1000000;
            //if (milisec >= delta * 1000)
            //{
            //    timer = hrc::now();
            //    MainUpdate(hWnd);
            //}

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
                    wndMouseDragOffset.x = ralsei->pos.x - cPos.x;
                    wndMouseDragOffset.y = ralsei->pos.y - cPos.y;
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
                    if (menu->InputEvent(Message, wParam, lParam));
                }
            }
            else
            {
                if (settings->IsOn())
                {
                    if (settings->IsInSubwindowRect(cPos))
                    {
                        if (settings->InputEvent(Message, wParam, lParam));
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
                        if (topicChoser->InputEvent(Message, wParam, lParam))
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

            WORD vkCode = LOWORD(wParam); // virtual-key code

            // MessageBoxA(NULL, to_string(wParam).c_str(), "", MB_OK);

            if (menu->IsOn())
            {
                menu->Off();
                ReleaseCapture();
            }
            else if (wParam == VK_F9)
            {
                menu->On();
                menu->SetMenuToMousePos();
                SetCapture(hWnd);
            }
            break;
        }

        case WM_KEYUP:
        {
            WORD vkCode = LOWORD(wParam); // virtual-key code
            if (vkCode == VK_APPS)
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
            GdiplusShutdown(gdiToken);
            PostQuitMessage(retCode);
            return retCode;
        }
    }
    return DefWindowProc(hWnd, Message, wParam, lParam);
}

void MainUpdate(HWND hWnd, double delta)
{
    // Handle window dragging
    if (mouseIsDown == 1)
    {
        POINT cPos;
        GetCursorPos(&cPos);

        ralsei->pos.x = cPos.x + wndMouseDragOffset.x;
        ralsei->pos.y = cPos.y + wndMouseDragOffset.y;
        ralsei->SetVelocity(0, 0);

        wndPosOld = cPos;
    }

    topicChoser->SetMenuPos(ralsei->pos.x - topicChoser->width / 2, ralsei->pos.y - 515);

    winMain->TreeUpdate(delta);
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
        RectF digrect(ralsei->pos.x - 250, ralsei->pos.y - 400, 500, 150);
        DrawFineRect(&g, &brush, digrect);
        g.DrawRectangle(&pen, digrect);

        // Draw text
        ATL::CString cstr = ralsei->GetCurrentSpeech();
        g.DrawString(cstr, wcslen(cstr), &f,
            PointF(ralsei->pos.x - 250 + 45, ralsei->pos.y - 400 + 10), &strformat, &textBrush);
        g.DrawString(L"*", wcslen(L"*"), &f,
            PointF(ralsei->pos.x - 250 + 15, ralsei->pos.y - 400 + 10), &strformat, &textBrush);
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