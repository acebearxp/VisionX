#include "stdafx.h"
#include "MainWnd.h"
#include "App.h"

CMainWnd::CMainWnd(CApp *pApp)
    :CXWnd(pApp)
{
    m_wstrClsName.assign(L"MainWnd");
}


CMainWnd::~CMainWnd()
{
}

ATOM CMainWnd::Register()
{
    HINSTANCE hInstance = m_pApp->GetInstance();

    WNDCLASS wcls;
    ZeroMemory(&wcls, sizeof(WNDCLASS));
    wcls.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcls.lpfnWndProc = CXWnd::WindowProc;
    wcls.hInstance = hInstance;
    wcls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wcls.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcls.lpszClassName = m_wstrClsName.c_str();
    wcls.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_APP));
    wcls.lpszMenuName = MAKEINTRESOURCE(IDR_MENU_MAIN);

    return RegisterClass(&wcls);
}

BOOL CMainWnd::Create()
{
    int cxScreen = GetSystemMetrics(SM_CXSCREEN);
    int cyScreen = GetSystemMetrics(SM_CYSCREEN);
    int width = cxScreen > 800 ? 800 : cxScreen;
    int height = cyScreen > 600 ? 600 : cyScreen;

    HINSTANCE hInstance = m_pApp->GetInstance();

    WNDCLASS cls;
    if (!GetClassInfo(hInstance, m_wstrClsName.c_str(), &cls)) Register();

    wstring wstrTitle(L"OpenCV");
    m_hwnd = CreateWindow(m_wstrClsName.c_str(), wstrTitle.c_str(), WS_OVERLAPPEDWINDOW|WS_VISIBLE,
        (cxScreen - width) / 2, (cyScreen - height) / 2, width, height,
        NULL, NULL, hInstance, this);
    if (!m_hwnd) {
        MessageBox(NULL, L"Create window failed", L"Error", MB_OK);
        return FALSE;
    }
    else {
        BOOL enable;
        DwmIsCompositionEnabled(&enable);
        if (enable) {
            DWMNCRENDERINGPOLICY policy = DWMNCRP_ENABLED;
            DwmSetWindowAttribute(m_hwnd, DWMWA_NCRENDERING_POLICY, &policy, sizeof(DWMNCRENDERINGPOLICY));
        }
    }

    RECT rc;
    GetClientRect(m_hwnd, &rc);

    return TRUE;
}

void CMainWnd::OnSize(HWND hwnd, UINT state, int cx, int cy)
{
    RECT rc;
    GetClientRect(m_hwnd, &rc);
}

void CMainWnd::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case ID_FILE_OPEN:
        PickImage();
        break;
    case ID_FILE_EXIT:
        PostQuitMessage(0);
        break;
    default:
        CXWnd::OnCommand(hwnd, id, hwndCtl, codeNotify);
        break;
    }
}

void CMainWnd::PickImage()
{
    wchar_t wcsFile[MAX_PATH] = L"\0";

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = m_hwnd;
    ofn.lpstrFilter = L"JPEG(*.jpg;*.jpeg)\0*.jpg;*.jpeg\0\0";
    ofn.lpstrFile = wcsFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_ENABLESIZING | OFN_EXPLORER;

    if (GetOpenFileName(&ofn)) {
        OutputDebugString(ofn.lpstrFile);
        // ofn.lpstrFile
    }
}
