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

    const wstring wstrTitle(L"Stitching");
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
    case WM_USER:
        InvalidateRect(hWnd, nullptr, TRUE);
        lRet = 0;
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

    // ˫����
    HDC hdc = GetDC(hwnd);
    m_hdcMem = CreateCompatibleDC(hdc);
    ReleaseDC(hwnd, hdc);

    m_uptrBrushBK = unique_ptr<Gdiplus::SolidBrush>(new Gdiplus::SolidBrush(Gdiplus::Color::WhiteSmoke));
    m_uptrPen = unique_ptr<Gdiplus::Pen>(new Gdiplus::Pen(Gdiplus::Color::Crimson));

    // ���������߳�
    InitializeCriticalSection(&m_cs);
    m_evPuls = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    m_thread = thread(&CMainWnd::doWork, this);

    return bRet;
}

void CMainWnd::OnDestroy(HWND hwnd)
{
    m_atomQuit = true;
    SetEvent(m_evPuls);
    m_thread.join();
    CloseHandle(m_evPuls);
    DeleteCriticalSection(&m_cs);

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

    // �ֳ���������,������ʾƴ�Ӻ��ͼ��,������ʾ������ͼ��
    EnterCriticalSection(&m_cs);
    const float fRatio = 0.8f; // �������ռ�ݱ���
    int count = m_mate40.GetCount();
    int posY = 0;
    // top
    if (count > 1) {
        Gdiplus::Rect rcMerge(rc.left, rc.top, rc.right - rc.left, static_cast<int>((rc.bottom - rc.top) * fRatio));
        const auto& uptrMerged = m_mate40.GetMergedBMP();
        resizeRectForImage(uptrMerged, rcMerge);
        rcMerge.Y = 0; // �����ƶ���������
        posY = rcMerge.Height;
        g.DrawImage(uptrMerged.get(), rcMerge);

        // separator
        g.DrawLine(m_uptrPen.get(), rc.left, posY, rc.right, posY);
    }

    // bottom
    if (count > 0) {
        int height = rc.bottom - rc.top - posY;
        int width = (rc.right - rc.left) / count;

        int i = 0;
        const auto& vuptrTeaPots = m_mate40.GetTeaPots();
        for (const auto& uptrTeaPot : vuptrTeaPots) {
            Gdiplus::Rect rcBmp(rc.left + width * i, posY, width, height);
            const auto& uptrBmp = uptrTeaPot->GetBMP();
            resizeRectForImage(uptrBmp, rcBmp);
            g.DrawImage(uptrBmp.get(), rcBmp);
            i++;
        }
    }
    LeaveCriticalSection(&m_cs);

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

void CMainWnd::pickImages()
{
    const int MAX_LEN = MAX_PATH * 2 + 1;
    wchar_t wcsFile[MAX_LEN] = L"\0";

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = m_hwnd;
    ofn.lpstrFilter = L"JPEG(*.jpg;*.jpeg)\0*.jpg;*.jpeg\0\0";
    ofn.lpstrFile = wcsFile;
    ofn.nMaxFile = MAX_LEN;
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

        // �����޳��ظ���
        auto append_unique = [this](const wstring & x) {
            for (const wstring& s : m_vImagePaths) {
                if (x == s) return;
            }
            m_vImagePaths.push_back(x);
        };

        size_t nBefore, nAfter;
        {
            EnterCriticalSection(&m_cs);
            nBefore = m_vImagePaths.size();
            if (selected.size() == 1) {
                append_unique(selected[0]);
            }
            else {
                for (int i = 1; i < selected.size(); i++) {
                    swprintf_s(wcsFile, L"%s\\%s", selected[0].c_str(), selected[i].c_str());
                    append_unique(wcsFile);
                }
            }
            nAfter = m_vImagePaths.size();
            LeaveCriticalSection(&m_cs);
        }
        if (nAfter > nBefore) {
            // ������߳�
            m_atomJob = true;
            SetEvent(m_evPuls);
        }
    }
}

void CMainWnd::clearImages()
{
    EnterCriticalSection(&m_cs);
    m_vImagePaths.clear();
    LeaveCriticalSection(&m_cs);
    // ������߳�
    m_atomJob = true;
    SetEvent(m_evPuls);
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

void CMainWnd::doWork()
{
    do {
        OutputDebugString(L"===> Puls...\n");

        if (m_atomJob) {
            vector<wstring> vImagePathsW;
            EnterCriticalSection(&m_cs);
            
            if (m_vImagePaths.size() > 0) {
                vImagePathsW = m_vImagePaths;
                m_atomJob = false;

                vector<string> vImagePathsA = convert(vImagePathsW);
                // װ��ͼ��
                m_mate40.LoadAll(vImagePathsA);

                // ����У��
                m_mate40.CalibrateForFisheye();

                // ͼ����׼
                m_mate40.SpaceMatching();
            }
            else {
                // clear
                m_mate40.clearAll();
            }

            LeaveCriticalSection(&m_cs);
        }
        PostMessage(m_hwnd, WM_USER, 0, 0);
        OutputDebugString(L"===> Wait...\n");
        WaitForSingleObject(m_evPuls, INFINITE);
    }
    while (!m_atomQuit);
    OutputDebugString(L"===> Quit...\n");
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
