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

    CoUninitialize();
}

int CApp::Run(PWSTR pCmdLine, int nCmdShow)
{
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}
