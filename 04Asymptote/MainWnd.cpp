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
    case WM_USER:
        InvalidateRect(m_hwnd, nullptr, FALSE);
        lRet = 0L;
        break;
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
    m_uptrPenGray = unique_ptr<Gdiplus::Pen>(new Gdiplus::Pen(Gdiplus::Color::Gray));

    // 启动工作线程
    m_evWakeUp = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    InitializeCriticalSectionEx(&m_csRX6K, 50, 0);
    m_thread = thread(&CMainWnd::doWork, this);

    return bRet;
}

void CMainWnd::OnDestroy(HWND hwnd)
{
    // 结束工作线程
    m_atomExit = true;
    SetEvent(m_evWakeUp);
    m_thread.join();
    DeleteCriticalSection(&m_csRX6K);
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

    EnterCriticalSection(&m_csRX6K);
    // top for output
    if (m_uptrOutputBitmap != nullptr) {
        auto& bmp = m_uptrOutputBitmap->GetBmp();
        Gdiplus::Rect rcBmp(rc.left, rc.top, rc.right-rc.left, nYSeparator - rc.top);
        resizeRectForImage(bmp, rcBmp);
        g.DrawImage(&bmp, rcBmp);
    }

    // bottom for each input image
    int count = static_cast<int>(m_vuptrBitmaps.size());
    if (count > 0) {
        int nYPos = nYSeparator + 1;
        int nWidth = (rc.right - rc.left) / count;
        for (int i = 0; i < m_vuptrBitmaps.size(); i++) {
            auto& bmp = m_vuptrBitmaps[i]->GetBmp();
            Gdiplus::Rect rcBmp(rc.left + nWidth * i, nYPos, nWidth, rc.bottom - nYPos);
            resizeRectForImage(bmp, rcBmp);
            g.DrawImage(&bmp, rcBmp);
        }
    }
    LeaveCriticalSection(&m_csRX6K);

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

    // 枚举显示器,默认显示在较大的显示器上
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

    // 如果前面的办法没有获取到,使用默认显示器参数
    if (rc.right - rc.left <= 0 || rc.bottom - rc.top <= 0) {
        rc.left = rc.top = 0;
        rc.right = GetSystemMetrics(SM_CXSCREEN);
        rc.bottom = GetSystemMetrics(SM_CYSCREEN);
    }

    // 最大不超过800x600
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
        // 多选文件,双0结尾
        vector<wstring> selected;
        wchar_t* pStart = ofn.lpstrFile;
        while (*pStart != L'\0') {
            selected.push_back(pStart);
            pStart += wcslen(pStart) + 1;
        }

        // 剔除重复项
        auto append_unique = [](vector<wstring>& vPaths, const wstring& s) {
            for (const wstring& path : vPaths) {
                if (path == s) return;
            }
            vPaths.push_back(s);
        };

        EnterCriticalSection(&m_csRX6K);
        if (selected.size() == 1) {
            append_unique(m_vPaths, selected[0]);
        }
        else {
            for (int i = 1; i < selected.size(); i++) {
                swprintf_s(wcsFile, L"%s\\%s", selected[0].c_str(), selected[i].c_str());
                append_unique(m_vPaths, wcsFile);
            }
        }
        LeaveCriticalSection(&m_csRX6K);
        SetEvent(m_evWakeUp);
    }
}

void CMainWnd::clearImages()
{
    EnterCriticalSection(&m_csRX6K);
    m_vPaths.clear();
    LeaveCriticalSection(&m_csRX6K);
    SetEvent(m_evWakeUp);
}

void CMainWnd::doWork()
{

    while (WAIT_OBJECT_0 == WaitForSingleObject(m_evWakeUp, INFINITE)) {
        if (m_atomExit) break;
        OutputDebugString(L"=== Thread ===> Working...\n");

        // 处理输入
        vector<float> vk(5);
        vector<wstring> vPaths = doWorkForInput(vk);

        // 装载图像
        m_rx6k.LoadImages(convert(vPaths));
        // 设定参数
        m_rx6k.SetFisheye(vk);

        // 输出一次中间结果
        doWorkForOutput(true, false);

        // 计算输出图像
        m_rx6k.Compute();

        // 输出最终结果
        doWorkForOutput(true, true);
        
    }
    OutputDebugString(L"=== Thread ===> Exit\n");
}

vector<wstring> CMainWnd::doWorkForInput(std::vector<float>& vk)
{
    vector<wstring> vPaths;
    EnterCriticalSection(&m_csRX6K);
    vPaths = m_vPaths;
    vk.clear();
    for (auto i = m_vk.begin(); i != m_vk.end(); i++) {
        vk.push_back(*i);
    }
    LeaveCriticalSection(&m_csRX6K);

#ifdef _DEBUG
    wchar_t buf[1024];
    for (const auto& s : vPaths) {
        swprintf_s(buf, L"=== vPaths ===> %s\n", s.c_str());
        OutputDebugString(buf);
    }
#endif // _DEBUG

    return vPaths;
}

void CMainWnd::doWorkForOutput(bool bUpdateMiddle, bool bUpdateFinal)
{
    vector<unique_ptr<CVBitmap>> vuptrBitmaps;
    unique_ptr<CVBitmap> uptrOutputBitmap;

    // 准备输出
    if (bUpdateMiddle) {
        for (const auto& uptrBeaker : m_rx6k.GetBeakers()) {
            const cv::Mat& image = uptrBeaker->GetImage();
            vuptrBitmaps.push_back(unique_ptr<CVBitmap>(new CVBitmap(image)));
        }
    }

    if (bUpdateFinal) {
        if (m_rx6k.GetOutputBeaker() != nullptr) {
            uptrOutputBitmap = make_unique<CVBitmap>(CVBitmap(m_rx6k.GetOutputBeaker()->GetImage()));
        }
    }

    // 处理输出
    EnterCriticalSection(&m_csRX6K);
    if(bUpdateMiddle) m_vuptrBitmaps = move(vuptrBitmaps);
    if(bUpdateFinal)  m_uptrOutputBitmap = move(uptrOutputBitmap);
    LeaveCriticalSection(&m_csRX6K);
    PostMessage(m_hwnd, WM_USER, 0, 0);
}

void CMainWnd::resizeRectForImage(Gdiplus::Bitmap& bmp, Gdiplus::Rect& rc)
{
    // 计算图片绘制区域 保持纵横比例不变
    int nWidthBMP = bmp.GetWidth(), nHeightBMP = bmp.GetHeight();
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

vector<string> CMainWnd::convert(const vector<wstring>& vSrcW)
{
    // convert to multi-bytes
    vector<string> vOutputA;

    char szbuf[1024];
    for (const wstring& s : vSrcW) {
        WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, s.c_str(), -1, szbuf, sizeof(szbuf), NULL, FALSE);
        vOutputA.push_back(szbuf);
    }

    return vOutputA;
}