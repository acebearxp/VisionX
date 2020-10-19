#pragma once
#include "XWnd.h"
#include "resource.h"
#include "CamCali.h"

class CMainWnd : public CXWnd
{
public:
    CMainWnd(CApp *pApp);
    virtual ~CMainWnd();

    virtual ATOM Register();
    BOOL Create();
protected:
    LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

    void OnPaint(HWND hwnd) override;
    void OnSize(HWND hwnd, UINT state, int cx, int cy) override;
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) override;

    void OnGetMinMaxInfo(HWND hwnd, LPMINMAXINFO lpMinMaxInfo);
private:
    bool m_bOpened = false;
    std::unique_ptr<CamCali> m_uptrCC;
    std::thread m_threadWorking;

    // 右栏分割宽度
    const int m_nRightWidth = 200;

    void pickImage();

    // 计算绘制区域
    void calcRectForImage(Gdiplus::Rect& rc);
};

