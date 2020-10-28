#include "stdafx.h"
#include "MainWnd.h"

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
    int cxScreen = GetSystemMetrics(SM_CXSCREEN);
    int cyScreen = GetSystemMetrics(SM_CYSCREEN);
    int width = cxScreen > 800 ? 800 : cxScreen;
    int height = cyScreen > 600 ? 600 : cyScreen;

    WNDCLASS cls;
    if (!GetClassInfo(hInstance, c_wszClsName, &cls)) Register(hInstance);

    const wstring wstrTitle(L"Asymptote");
    m_hwnd = CreateWindow(c_wszClsName, wstrTitle.c_str(), WS_OVERLAPPEDWINDOW|WS_VISIBLE,
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

    // 双缓冲
    HDC hdc = GetDC(hwnd);
    m_hdcMem = CreateCompatibleDC(hdc);
    ReleaseDC(hwnd, hdc);

    m_uptrBrushBK = unique_ptr<Gdiplus::SolidBrush>(new Gdiplus::SolidBrush(Gdiplus::Color::WhiteSmoke));

    return bRet;
}

void CMainWnd::OnDestroy(HWND hwnd)
{
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
        pickImage();
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

void CMainWnd::pickImage()
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
        // convert to multi-bytes
        char szbuf[MAX_PATH + 1];
        memset(szbuf, 0, sizeof(szbuf));
        WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, ofn.lpstrFile, static_cast<int>(wcslen(ofn.lpstrFile)), szbuf, MAX_PATH, NULL, FALSE);
    }
}

void CMainWnd::resizeRectForImage(const unique_ptr<Gdiplus::Bitmap>& uptrBMP, Gdiplus::Rect& rc)
{
    // 计算图片绘制区域 保持纵横比例不变
    int nWidthBMP = uptrBMP->GetWidth(), nHeightBMP = uptrBMP->GetHeight();
    float fRatioRC = 1.0f * rc.Width / rc.Height;

    float fRatioIMG = 1.0f * nWidthBMP / nHeightBMP;

    if (fRatioRC >= fRatioIMG) {
        // 绘制区域比图像宽,图像垂直方向占满
        float fWidth = 1.0f * nWidthBMP * rc.Height / nHeightBMP;
        rc.X = static_cast<int>(rc.X + (rc.Width - fWidth) / 2.0f);
        rc.Width = static_cast<int>(fWidth);
    }
    else {
        // 绘制区域比图像窄,图像水平方向占满
        float fHeight = 1.0f * nHeightBMP * rc.Width / nWidthBMP;
        rc.Y = static_cast<int>(rc.Y + (rc.Height - fHeight) / 2.0f);
        rc.Height = static_cast<int>(fHeight);
    }
}