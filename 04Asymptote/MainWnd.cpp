#include "stdafx.h"
#include "MainWnd.h"

using namespace std;

const wchar_t CMainWnd::c_wszClsName[] = L"CMainWnd";

ATOM CMainWnd::Register(HINSTANCE hInstance)
{
    WNDCLASS wcls;
    ZeroMemory(&wcls, sizeof(WNDCLASS));
    wcls.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcls.lpfnWndProc = CXWnd::WindowProc;
    wcls.hInstance = hInstance;
    wcls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wcls.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcls.lpszClassName = c_wszClsName;
    wcls.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_APP));
    wcls.lpszMenuName = MAKEINTRESOURCE(IDR_MENU_MAIN);

    return RegisterClass(&wcls);
}

BOOL CMainWnd::Create(HINSTANCE hInstance)
{
    const RECT rc = calcDefaultWindowRect();

    WNDCLASS cls;
    if (!GetClassInfo(hInstance, c_wszClsName, &cls)) Register(hInstance);

    const wstring wstrTitle(L"Asymptote");
    m_hwnd = CreateWindow(c_wszClsName, wstrTitle.c_str(), WS_OVERLAPPEDWINDOW|WS_VISIBLE,
        rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top,
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

    return TRUE;
}

LRESULT CMainWnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRet;
    switch (uMsg)
    {
        HANDLE_MSG(hWnd, WM_ERASEBKGND, OnEraseBkgnd);
        HANDLE_MSG(hWnd, WM_SIZE, OnSize);
        HANDLE_MSG(hWnd, WM_GETMINMAXINFO, OnGetMinMaxInfo);
    default:
        lRet = CXWnd::WndProc(hWnd, uMsg, wParam, lParam);
        break;
    }

    return lRet;
}

BOOL CMainWnd::OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    BOOL bRet = CXWnd::OnCreate(hwnd, lpCreateStruct);

    // ˫����
    HDC hdc = GetDC(hwnd);
    m_hdcMem = CreateCompatibleDC(hdc);
    ReleaseDC(hwnd, hdc);

    m_uptrBrushBK = unique_ptr<Gdiplus::SolidBrush>(new Gdiplus::SolidBrush(Gdiplus::Color::WhiteSmoke));
    m_uptrPenGray = unique_ptr<Gdiplus::Pen>(new Gdiplus::Pen(Gdiplus::Color::Gray));

    // ���������߳�
    m_evWakeUp = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    InitializeCriticalSectionEx(&m_csInput, 50, 0);
    m_thread = thread(&CMainWnd::doWork, this);

    return bRet;
}

void CMainWnd::OnDestroy(HWND hwnd)
{
    // ���������߳�
    m_atomExit = true;
    SetEvent(m_evWakeUp);
    m_thread.join();
    DeleteCriticalSection(&m_csInput);
    CloseHandle(m_evWakeUp);

    ReleaseDC(hwnd, m_hdcMem);
    DeleteBitmap(m_hBmpMem);
    CXWnd::OnDestroy(hwnd);

    PostQuitMessage(0);
}

void CMainWnd::OnPaint(HWND hwnd)
{
    RECT rc;
    GetClientRect(hwnd, &rc);

    Gdiplus::Graphics g(m_hdcMem);
    g.FillRectangle(m_uptrBrushBK.get(), 0, 0, rc.right, rc.bottom);

    // separator
    const int nYSeparator = static_cast<int>((rc.bottom - rc.top) * m_fSeparator);
    g.DrawLine(m_uptrPenGray.get(), rc.left, nYSeparator, rc.right, nYSeparator);

    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    BitBlt(hdc, 0, 0, rc.right, rc.bottom, m_hdcMem, 0, 0, SRCCOPY);
    EndPaint(hwnd, &ps);
}

BOOL CMainWnd::OnEraseBkgnd(HWND hwnd, HDC hdc)
{
    return TRUE;
}

void CMainWnd::OnSize(HWND hwnd, UINT state, int cx, int cy)
{
    HDC hdc = GetDC(hwnd);
    m_hBmpMem = CreateCompatibleBitmap(hdc, cx, cy);
    HBITMAP hBmpOld = SelectBitmap(m_hdcMem, m_hBmpMem);
    DeleteBitmap(hBmpOld);
}

void CMainWnd::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case ID_FILE_OPEN:
        pickImages();
        break;
    case ID_FILE_CLOSE:
        clearImages();
        break;
    case ID_FILE_EXIT:
        SendMessage(hwnd, WM_CLOSE, 0, 0);
        break;
    default:
        CXWnd::OnCommand(hwnd, id, hwndCtl, codeNotify);
        break;
    }
}

void CMainWnd::OnGetMinMaxInfo(HWND hwnd, LPMINMAXINFO lpMinMaxInfo)
{
    lpMinMaxInfo->ptMinTrackSize.x = 400;
    lpMinMaxInfo->ptMinTrackSize.y = 300;
}

RECT CMainWnd::calcDefaultWindowRect()
{
    RECT rc;

    // ö����ʾ��,Ĭ����ʾ�ڽϴ����ʾ����
    EnumDisplayMonitors(NULL, nullptr,
        [](HMONITOR hMonitor, HDC hdc, LPRECT pRECT, LPARAM pParam)->BOOL
    {
        RECT& rc = *reinterpret_cast<RECT*>(pParam);
        float fArea = 1.0f * (rc.right - rc.left) * (rc.bottom - rc.top);
        if (1.0f * (pRECT->right - pRECT->left) * (pRECT->bottom - pRECT->top) > fArea) {
            rc = *pRECT;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&rc));

    // ���ǰ��İ취û�л�ȡ��,ʹ��Ĭ����ʾ������
    if (rc.right - rc.left <= 0 || rc.bottom - rc.top <= 0) {
        rc.left = rc.top = 0;
        rc.right = GetSystemMetrics(SM_CXSCREEN);
        rc.bottom = GetSystemMetrics(SM_CYSCREEN);
    }

    // ��󲻳���800x600
    const int maxWidth = 800, maxHeight = 600;
    if (rc.right - rc.left > maxWidth) {
        int center = (rc.left + rc.right) / 2;
        rc.left = center - maxWidth / 2;
        rc.right = center + maxWidth / 2;
    }
    if (rc.bottom - rc.top > maxHeight) {
        int center = (rc.top + rc.bottom) / 2;
        rc.top = center - maxHeight / 2;
        rc.bottom = center + maxHeight / 2;
    }

    return rc;
}

void CMainWnd::pickImages()
{
    wchar_t wcsFile[MAX_PATH*2] = L"\0";

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = m_hwnd;
    ofn.lpstrFilter = L"JPEG(*.jpg;*.jpeg)\0*.jpg;*.jpeg\0\0";
    ofn.lpstrFile = wcsFile;
    ofn.nMaxFile = sizeof(wcsFile);
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_ENABLESIZING | OFN_EXPLORER | OFN_ALLOWMULTISELECT;

    if (GetOpenFileName(&ofn)) {
        // ��ѡ�ļ�,˫0��β
        vector<wstring> selected;
        wchar_t* pStart = ofn.lpstrFile;
        while (*pStart != L'\0') {
            selected.push_back(pStart);
            pStart += wcslen(pStart) + 1;
        }

        // �޳��ظ���
        auto append_unique = [](vector<wstring>& vPaths, const wstring& s) {
            for (const wstring& path : vPaths) {
                if (path == s) return;
            }
            vPaths.push_back(s);
        };

        EnterCriticalSection(&m_csInput);
        if (selected.size() == 1) {
            append_unique(m_vPaths, selected[0]);
        }
        else {
            for (int i = 1; i < selected.size(); i++) {
                swprintf_s(wcsFile, L"%s\\%s", selected[0].c_str(), selected[i].c_str());
                append_unique(m_vPaths, wcsFile);
            }
        }
        LeaveCriticalSection(&m_csInput);
        SetEvent(m_evWakeUp);
    }
}

void CMainWnd::clearImages()
{
    EnterCriticalSection(&m_csInput);
    m_vPaths.clear();
    LeaveCriticalSection(&m_csInput);
    SetEvent(m_evWakeUp);
}

void CMainWnd::doWork()
{
    while (WAIT_OBJECT_0 == WaitForSingleObject(m_evWakeUp, INFINITE)) {
        if (m_atomExit) break;
        OutputDebugString(L"=== Thread ===> Working...\n");

        EnterCriticalSection(&m_csInput);
        wchar_t buf[1024];
        for (const auto& s : m_vPaths) {
            swprintf_s(buf, L"=== m_vPath ===> %s\n", s.c_str());
            OutputDebugString(buf);
        }
        LeaveCriticalSection(&m_csInput);
    }
    OutputDebugString(L"=== Thread ===> Exit\n");
}

void CMainWnd::resizeRectForImage(const unique_ptr<Gdiplus::Bitmap>& uptrBMP, Gdiplus::Rect& rc)
{
    // ����ͼƬ�������� �����ݺ��������
    int nWidthBMP = uptrBMP->GetWidth(), nHeightBMP = uptrBMP->GetHeight();
    float fRatioRC = 1.0f * rc.Width / rc.Height;

    float fRatioIMG = 1.0f * nWidthBMP / nHeightBMP;

    if (fRatioRC >= fRatioIMG) {
        // ���������ͼ���,ͼ��ֱ����ռ��
        float fWidth = 1.0f * nWidthBMP * rc.Height / nHeightBMP;
        rc.X = static_cast<int>(rc.X + (rc.Width - fWidth) / 2.0f);
        rc.Width = static_cast<int>(fWidth);
    }
    else {
        // ���������ͼ��խ,ͼ��ˮƽ����ռ��
        float fHeight = 1.0f * nHeightBMP * rc.Width / nWidthBMP;
        rc.Y = static_cast<int>(rc.Y + (rc.Height - fHeight) / 2.0f);
        rc.Height = static_cast<int>(fHeight);
    }
}