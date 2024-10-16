#include "Platform.h"

// Constructor
Platform::Platform()
{
    m_windowClassName = L"Direct3DWindowClass";
    m_hInstance = NULL;
}

// Create a window
HRESULT Platform::CreatePlatform(WNDPROC customProc)
{
    if (m_hInstance == NULL)
        m_hInstance = (HINSTANCE)GetModuleHandle(NULL);

    HICON hIcon = NULL;
    WCHAR szExePath[MAX_PATH];
    GetModuleFileName(NULL, szExePath, MAX_PATH);


    // Class name, icon, cursor
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
    wClass.hInstance = m_hInstance;
    wClass.lpfnWndProc = customProc != NULL ? customProc : Platform::StaticWindowProc;
    wClass.lpszClassName = m_windowClassName.c_str();
    wClass.style = NULL;

    if (!RegisterClassEx(&wClass))
    {
        DWORD dwError = GetLastError();
        MessageBox(NULL, L"Error registering class!", std::to_wstring(dwError).c_str(), MB_OK);

        if (dwError != ERROR_CLASS_ALREADY_EXISTS)
            return HRESULT_FROM_WIN32(dwError);
    }

    m_rc;
    int x = CW_USEDEFAULT;
    int y = CW_USEDEFAULT;

    // Get the size of the windows
    int nWidth = GetSystemMetrics(SM_CXSCREEN);
    int nHeight = GetSystemMetrics(SM_CYSCREEN) - 1;
    SetRect(&m_rc, 0, 0, nWidth, nHeight);

    // Create the window for our viewport.
    m_hWnd = CreateWindowExW
    (
        WS_EX_TOPMOST | WS_EX_LAYERED,
        m_windowClassName.c_str(),
        L"Hello!",
        WS_POPUP,
        0, 0, nWidth, nHeight,
        NULL,
        NULL,
        m_hInstance,
        NULL
    );

    if (m_hWnd == NULL)
    {
        DWORD dwError = GetLastError();
        MessageBox(NULL, L"Error windows!", std::to_wstring(dwError).c_str(), MB_OK);
        return HRESULT_FROM_WIN32(dwError);
    }

    ShowWindow(m_hWnd, SW_SHOW);
    if (!SetForegroundWindow(m_hWnd)) MessageBox(NULL, L"Can't bring to front", L"", MB_OK);
    return S_OK;
}


HRESULT Platform::Run()
{
    HRESULT hr = S_OK;

    if (!IsWindowVisible(m_hWnd))
        ShowWindow(m_hWnd, SW_SHOW);

    // The render loop is controlled here.
    bool bGotMsg;
    MSG  msg;
    msg.message = WM_NULL;
    PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

    while (WM_QUIT != msg.message)
    {
        // Process window events.
        // Use PeekMessage() so we can use idle time to render the scene. 
        bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);

        if (bGotMsg)
        {
            // Translate and dispatch the message
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // Update the scene.

            // Render frames during idle time (when no messages are waiting).

            // Present the frame to the screen.
        }
    }

    return hr;
}


// Destructor.
Platform::~Platform()
{

}


// Process windows messages. This looks for window close events, letting us
// exit out of the sample.
LRESULT CALLBACK Platform::StaticWindowProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (uMsg)
    {
    case WM_CLOSE:
    {
        HMENU hMenu;
        hMenu = GetMenu(hWnd);
        if (hMenu != NULL)
        {
            DestroyMenu(hMenu);
        }
        DestroyWindow(hWnd);
        UnregisterClass(
            m_windowClassName.c_str(),
            m_hInstance
        );
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}