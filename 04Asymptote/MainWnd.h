#pragma once
#include "XWnd.h"
#include "resource.h"

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
    // 分割线位置
    const float m_fSeparator = 0.8f;

    // GDI+资源
    HDC m_hdcMem;
    HBITMAP m_hBmpMem;
    std::unique_ptr<Gdiplus::SolidBrush> m_uptrBrushBK;
    std::unique_ptr<Gdiplus::Pen> m_uptrPenGray;

    // 计算默认窗口位置
    RECT calcDefaultWindowRect();
    // 打开图像
    void pickImages();
    // 关闭图像
    void clearImages();
    // 计算图像绘制区域
    void calcRectForImage(Gdiplus::Rect& rc);
private:
    // 工作信号
    HANDLE m_evWakeUp;
    // 退出标志
    std::atomic_bool m_atomExit;
    // 工作线程
    std::thread m_thread;

    // 保护
    CRITICAL_SECTION m_csInput;
    // 待处理图片路径
    std::vector<std::wstring> m_vPaths;

    // 后台任务
    void doWork();
private:
    static const wchar_t c_wszClsName[];
    // 调整图像绘制区域以保持原始比例
    static void resizeRectForImage(const std::unique_ptr<Gdiplus::Bitmap>& uptrBMP, Gdiplus::Rect& rc);
};

