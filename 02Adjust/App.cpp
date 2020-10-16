#include "stdafx.h"
#include "App.h"

CApp::CApp()
{
    m_hInstance = NULL;
}


CApp::~CApp()
{
}

BOOL CApp::Init(HINSTANCE hInstance)
{
    m_hInstance = hInstance;

    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) return FALSE;

    // GDI+
    Gdiplus::GdiplusStartupInput startup;
    Gdiplus::GdiplusStartup(&m_token, &startup, NULL);

    // create MainWnd
    m_pwndMain = new CMainWnd(this);
    if(!m_pwndMain->Create()) return FALSE;

    return TRUE;
}

void CApp::UnInit()
{
    // destroy MainWnd
    delete m_pwndMain;
    m_pwndMain = NULL;

    Gdiplus::GdiplusShutdown(m_token);

    CoUninitialize();
}

int CApp::Run(PWSTR pCmdLine, int nCmdShow)
{
    HACCEL hAccel = LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!TranslateAccelerator(*m_pwndMain, hAccel, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    DestroyAcceleratorTable(hAccel);

    return static_cast<int>(msg.wParam);
}
