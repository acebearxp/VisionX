#pragma once
#include "XWnd.h"
#include "resource.h"

using namespace std;

class CMainWnd final : public CXWnd
{
public:
    CMainWnd() {};
    ~CMainWnd() override {};

    ATOM Register(HINSTANCE hInstance) override;
    BOOL Create(HINSTANCE hInstance);
protected:
    LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

    BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct) override;
    void OnDestroy(HWND hwnd) override;
    void OnPaint(HWND hwnd) override;
    BOOL OnEraseBkgnd(HWND hwnd, HDC hdc);
    void OnSize(HWND hwnd, UINT state, int cx, int cy);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) override;

    void OnGetMinMaxInfo(HWND hwnd, LPMINMAXINFO lpMinMaxInfo);
private:
    

    // GDI+资源
    HDC m_hdcMem;
    HBITMAP m_hBmpMem;
    unique_ptr<Gdiplus::SolidBrush> m_uptrBrushBK;

    // 计算默认窗口位置
    RECT calcDefaultWindowRect();
    // 打开图像
    void pickImage();
    // 计算图像绘制区域
    void calcRectForImage(Gdiplus::Rect& rc);
private:
    static const wchar_t c_wszClsName[];
    // 调整图像绘制区域以保持原始比例
    static void resizeRectForImage(const unique_ptr<Gdiplus::Bitmap>& uptrBMP, Gdiplus::Rect& rc);
};

