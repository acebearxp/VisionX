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
    void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
    void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
    void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
private:
    // 尺寸系数
    const int m_nRightWidth = 200; // 右栏分割宽度
    const int m_nMargin = 4; // 边距
    float m_fLineHeight = -1.0f; // 行高,字体相关

    // GDI+资源
    std::unique_ptr<Gdiplus::Font> m_uptrFont;
    std::unique_ptr<Gdiplus::SolidBrush> m_uptrBrush;
    std::unique_ptr<Gdiplus::SolidBrush> m_uptrBrushFocus;
    std::unique_ptr<Gdiplus::Pen> m_uptrPen;

    bool m_bOpened = false; // 图像已经打开标志
    int m_nTracking = 0; // 当前滑块跟踪索引 0焦距 1~4系数K

    Gdiplus::Point m_pointA, m_pointB;
    Gdiplus::Rect m_rcBtn;
    bool m_bTracking = false;
    int m_nTrackingYDelta;

    // 系数
    float m_fFocus = 500.0f;
    std::vector<float> m_fK4 = {2.0f, 3.0f, 1.0f, 0.0f};

    // 镜头修正处理对象
    std::unique_ptr<CamCali> m_uptrCC;

    // 打开图像
    void pickImage();
    // 计算图像绘制区域
    void calcRectForImage(Gdiplus::Rect& rc);
    // 计算焦距位置百分比
    float calcFocusPos();
    bool calcFocusPosBack(int y);
    // 计算系数K位置百分比
    float calcKPos(int n);
    bool calcKPosBack(int y);
};

