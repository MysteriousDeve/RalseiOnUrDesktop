#include "Platform.h"

// Static decl
Platform* Platform::topParent = nullptr;

// Constructor
Platform::Platform() : Subwindow::Subwindow()
{
    On();
    Platform::topParent = this;
    m_windowClassName = L"what the fuck bro";
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

    int x = CW_USEDEFAULT;
    int y = CW_USEDEFAULT;

    // Get the size of the windows
    // Old method (not accounting for the taskbar)
        // int nWidth = GetSystemMetrics(SM_CXSCREEN);
        // int nHeight = GetSystemMetrics(SM_CYSCREEN) - 1;
        // SetRect(&m_rc, 0, 0, nWidth, nHeight);
    // New method
    int wHeight = GetSystemMetrics(SM_CYSCREEN);
    SystemParametersInfoA(SPI_GETWORKAREA, 0, &m_rc, 0);
    int nWidth = m_rc.right - m_rc.left;
    int nHeight = m_rc.bottom - m_rc.top;
    if (wHeight == nHeight)
    {
        m_rc.bottom -= 1;
        nHeight -= 1;
    }
    m_windowSize = Vector2Int(nWidth, nHeight);
    size = m_windowSize;

    // Create the window for our viewport.
    m_hWnd = CreateWindowExW
    (
        WS_EX_TOPMOST | WS_EX_LAYERED,
        m_windowClassName.c_str(),
        L"Ralsei on your Desktop!",
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
    // AddThumbarButtons(m_hWnd);
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

    typedef std::chrono::high_resolution_clock hrc;
    static auto timer = hrc::now();

    while (WM_QUIT != msg.message)
    {
        // efficiencyMode = forceEfficiencyMode || !isDeviceCharging();
        if (false)
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
                long double milisec = (hrc::now() - timer).count() / (long double)1000000;
                if (milisec >= 1 / (long double)60.0 * 1000)
                {
                    _Update(milisec / 1000);
                    timer = hrc::now();
                }
            }
        }
    }

    return hr;
}


HRESULT Platform::Render()
{
    return 0;
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

Vector2Int Platform::GetDimension()
{
    return m_windowSize;
}


HRESULT Platform::AddThumbarButtons(HWND hwnd)
{
    HIMAGELIST hImageList = ImageList_LoadBitmap(m_hInstance, MAKEINTRESOURCEW(IDI_SMALL), 16, 0, RGB(255, 0, 255));
    ImageList_Add(hImageList, LoadBitmap(m_hInstance, MAKEINTRESOURCE(IDI_SMALL)), NULL);

    // Define an array of two buttons. These buttons provide images through an 
    // image list and also provide tooltips.
    THUMBBUTTONMASK dwMask = THB_BITMAP | THB_FLAGS;

    THUMBBUTTON thbButtons[2] = {};
    thbButtons[0].dwMask = dwMask;
    thbButtons[0].iId = 0;
    thbButtons[0].iBitmap = 0;
    thbButtons[0].dwFlags = THBF_DISMISSONCLICK;

    dwMask = THB_BITMAP | THB_TOOLTIP;
    thbButtons[1].dwMask = dwMask;
    thbButtons[1].iId = 1;
    thbButtons[1].iBitmap = 1;

    // Create an instance of ITaskbarList3
    ITaskbarList3* ptbl;
    HRESULT hr = CoCreateInstance(CLSID_TaskbarList,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&ptbl)
    );

    if (SUCCEEDED(hr))
    {
        // Declare the image list that contains the button images.
        hr = ptbl->ThumbBarSetImageList(hwnd, hImageList);

        if (true)//SUCCEEDED(hr))
        {
            // Attach the toolbar to the thumbnail.
            hr = ptbl->ThumbBarAddButtons(hwnd, ARRAYSIZE(thbButtons), thbButtons);
        }
        else
        {
            DWORD dwError = GetLastError();
            MessageBox(NULL, L"Error thumbbar!", std::to_wstring(dwError).c_str(), MB_OK);
            exit(-1);
        }
        ptbl->Release();
    }
    return hr;
}
