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

    // 字体
    Gdiplus::FontFamily ff(L"宋体");
    m_uptrFont = std::unique_ptr<Gdiplus::Font>(new Gdiplus::Font(&ff, 12));
    // 画刷
    m_uptrBrush = std::unique_ptr<Gdiplus::SolidBrush>(new Gdiplus::SolidBrush(Gdiplus::Color::Black));
    m_uptrBrushFocus = std::unique_ptr<Gdiplus::SolidBrush>(new Gdiplus::SolidBrush(Gdiplus::Color::Crimson));
    // 画笔
    m_uptrPen = std::unique_ptr<Gdiplus::Pen>(new Gdiplus::Pen(Gdiplus::Color::Black));

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
        HANDLE_MSG(hWnd, WM_MOUSEMOVE, OnMouseMove);
        HANDLE_MSG(hWnd, WM_LBUTTONDOWN, OnLButtonDown);
        HANDLE_MSG(hWnd, WM_LBUTTONUP, OnLButtonUp);
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
    
    // 获取行高
    if (m_fLineHeight < 0) m_fLineHeight = static_cast<float>(m_uptrFont->GetHeight(&g));

    // 分左右栏
    int posDiv = rc.right - m_nRightWidth;
    g.DrawLine(m_uptrPen.get(), posDiv, 0, posDiv, rc.bottom);

    if (m_bOpened) {
        // 左栏绘图
        Gdiplus::Rect rcImage(0, 0, rc.right-m_nRightWidth, rc.bottom);
        calcRectForImage(rcImage);
        Gdiplus::Bitmap* pBMP = m_uptrCC->GetBMP();
        g.DrawImage(pBMP, rcImage);
    }

    // 右栏调节参数
    wchar_t wszBuf[64];
    Gdiplus::Brush* pBrush;

    Gdiplus::PointF point(static_cast<float>(rc.right - m_nRightWidth + m_nMargin), 1.0f * m_nMargin);
    float fDeltaH = m_fLineHeight + m_nMargin;

    swprintf_s(wszBuf, L"%3s %4.2f", L"焦距", m_fFocus);
    pBrush = m_nTracking == 0 ? m_uptrBrushFocus.get() : m_uptrBrush.get();
    g.DrawString(wszBuf, -1, m_uptrFont.get(), point, pBrush);

    for (int i = 0; i < 4; i++) {
        swprintf_s(wszBuf, L"%4s%d %4.2f", L"K", i + 1, m_fK4[i]);
        pBrush = m_nTracking == i + 1 ? m_uptrBrushFocus.get() : m_uptrBrush.get();
        point.Y += fDeltaH;
        g.DrawString(wszBuf, -1, m_uptrFont.get(), point, pBrush);
    }

    // 调节滑块
    const int side = 16; // 滑块边长
    m_pointA = Gdiplus::Point(rc.right - m_nRightWidth / 2, static_cast<int>(point.Y + fDeltaH));
    m_pointB = Gdiplus::Point(rc.right - m_nRightWidth / 2, static_cast<int>(rc.bottom - m_nMargin));
    g.DrawLine(m_uptrPen.get(), m_pointA, m_pointB);

    float fTracking;
    if (m_nTracking == 0) { // 焦距
        fTracking = calcFocusPos();
    }
    else { // 系数K
        fTracking = calcKPos(m_nTracking - 1);
    }
    float posY = fTracking * (m_pointB.Y - m_pointA.Y - side) + m_pointA.Y;

    m_rcBtn = Gdiplus::Rect(m_pointA.X - side / 2, static_cast<int>(posY), side, side);
    pBrush = m_bTracking ? m_uptrBrushFocus.get() : m_uptrBrush.get();
    g.FillRectangle(pBrush, m_rcBtn);

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

void CMainWnd::OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    if (x >= m_rcBtn.GetLeft() && x <= m_rcBtn.GetRight()
        && y >= m_rcBtn.GetTop() && y <= m_rcBtn.GetBottom()) {
        // hit
        SetCapture(hwnd);
        m_bTracking = true;
        m_nTrackingYDelta = m_rcBtn.Y - y;

        RECT rc;
        rc.left = m_rcBtn.GetLeft();
        rc.right = m_rcBtn.GetRight();
        rc.top = m_rcBtn.GetTop();
        rc.bottom = m_rcBtn.GetBottom();
        InvalidateRect(hwnd, &rc, TRUE);
    }
}

void CMainWnd::OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
    RECT rc;
    GetClientRect(hwnd, &rc);
    if (m_fLineHeight > 0 && x > rc.right - m_nRightWidth) {
        // 点中了右侧区域
        int nHit = y / (static_cast<int>(m_fLineHeight) + m_nMargin);
        if (nHit < 5 && m_nTracking != nHit) {
            // 切换
            m_nTracking = nHit;
            rc.left = rc.right - m_nRightWidth;
            InvalidateRect(hwnd, &rc, TRUE);
        }
    }

    if (m_bTracking) {
        ReleaseCapture();
        m_bTracking = false;
        rc.left = m_rcBtn.GetLeft();
        rc.right = m_rcBtn.GetRight();
        rc.top = m_rcBtn.GetTop();
        rc.bottom = m_rcBtn.GetBottom();
        InvalidateRect(hwnd, &rc, TRUE);
    }
}

void CMainWnd::OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
    if (m_bTracking) {
        wchar_t buf[64];
        swprintf_s(buf, L"===> %d : %d\n", m_nTrackingYDelta, y);
        OutputDebugString(buf);

        int yNew = y + m_nTrackingYDelta;

        bool bChanged = false;
        if (m_nTracking == 0)
            bChanged = calcFocusPosBack(yNew);
        else
            bChanged = calcKPosBack(yNew);

        if (bChanged) {
            RECT rc;
            rc.left = m_rcBtn.GetLeft();
            rc.right = m_rcBtn.GetRight();
            rc.top = m_rcBtn.GetTop();
            rc.bottom = m_rcBtn.GetBottom();
            InvalidateRect(hwnd, &rc, FALSE);

            GetClientRect(hwnd, &rc);
            rc.left = rc.right - m_nRightWidth;
            rc.top = m_nMargin + (m_nMargin + static_cast<int>(m_fLineHeight)) * m_nTracking;
            rc.bottom = rc.top + m_nMargin + static_cast<int>(m_fLineHeight);
            InvalidateRect(hwnd, &rc, TRUE);
        }
    }
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

float CMainWnd::calcFocusPos()
{
    // 1~3000
    const float min = 1.0f, max = 3000.0f;
    return (m_fFocus - min) / (max - min);
}

bool CMainWnd::calcFocusPosBack(int y)
{
    // wchar_t buf[64];
    // swprintf_s(buf, L"===> %d\n", nDeltaY);
    // OutputDebugString(buf);

    const float min = 1.0f, max = 3000.0f;
    if (y >= m_pointA.Y && y <= m_pointB.Y) {
        m_fFocus = (max - min) * (y - m_pointA.Y) / (m_pointB.Y - m_pointA.Y) + min;
        return true;
    }
    return false;
}

float CMainWnd::calcKPos(int n)
{
    // -50~+50
    const float min = -50.0f, max = 50.0f;
    return (m_fK4[n]-min) / (max - min);
}

bool CMainWnd::calcKPosBack(int y)
{
    const float min = -50.0f, max = 50.0f;
    if (y >= m_pointA.Y && y <= m_pointB.Y) {
        m_fK4[m_nTracking - 1] = (max - min) * (y - m_pointA.Y) / (m_pointB.Y - m_pointA.Y) + min;
        return true;
    }
    return false;
}
