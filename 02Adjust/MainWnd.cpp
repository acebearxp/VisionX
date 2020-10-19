#include "stdafx.h"
#include "MainWnd.h"
#include "App.h"

CMainWnd::CMainWnd(CApp *pApp)
    :CXWnd(pApp)
{
    m_wstrClsName.assign(L"MainWnd");
    m_uptrCC = std::unique_ptr<CamCali>(new CamCali());
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

    return TRUE;
}

LRESULT CMainWnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRet;
    switch (uMsg)
    {
        HANDLE_MSG(hWnd, WM_GETMINMAXINFO, OnGetMinMaxInfo);
    default:
        lRet = CXWnd::WndProc(hWnd, uMsg, wParam, lParam);
        break;
    }

    return lRet;
}

void CMainWnd::OnPaint(HWND hwnd)
{
    RECT rc;
    GetClientRect(hwnd, &rc);

    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    Gdiplus::Graphics g(hdc);

    // 分左右栏
    int posDiv = rc.right - m_nRightWidth;
    Gdiplus::Pen penBlack(Gdiplus::Color::Black);
    g.DrawLine(&penBlack, posDiv, 0, posDiv, rc.bottom);

    if (m_bOpened) {
        // 左栏绘图
        Gdiplus::Rect rcImage(0, 0, rc.right-m_nRightWidth, rc.bottom);
        calcRectForImage(rcImage);
        Gdiplus::Bitmap* pBMP = m_uptrCC->GetBMP();
        g.DrawImage(pBMP, rcImage);
    }

    EndPaint(hwnd, &ps);
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
        pickImage();
        break;
    case ID_FILE_EXIT:
        PostQuitMessage(0);
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

        // reset
        if (m_uptrCC != nullptr) m_uptrCC.reset();

        // create the CamCali(Camera Calibration)
        m_uptrCC = std::unique_ptr<CamCali>(new CamCali());
        m_uptrCC->OpenCameraImage(szbuf);
        m_bOpened = true;
        // refresh screen
        InvalidateRect(m_hwnd, NULL, TRUE);
    }
}

void CMainWnd::calcRectForImage(Gdiplus::Rect& rc)
{
    // 计算图片绘制区域 保持纵横比例不变
    float fRatioRC = 1.0f * rc.Width / rc.Height;

    Gdiplus::Bitmap *pBMP = m_uptrCC->GetBMP();
    float fRatioIMG = 1.0f * pBMP->GetWidth() / pBMP->GetHeight();

    if (fRatioRC >= fRatioIMG) {
        // 绘制区域比图像宽,图像垂直方向占满
        float fWidth = 1.0f * pBMP->GetWidth() * rc.Height / pBMP->GetHeight();
        rc.X = static_cast<int>((rc.Width - fWidth) / 2.0f);
        rc.Width = static_cast<int>(fWidth);
    }
    else {
        // 绘制区域比图像窄,图像水平方向占满
        float fHeight = 1.0f * pBMP->GetHeight() * rc.Width / pBMP->GetWidth();
        rc.Y = static_cast<int>((rc.Height - fHeight) / 2.0f);
        rc.Height = static_cast<int>(fHeight);
    }
}
