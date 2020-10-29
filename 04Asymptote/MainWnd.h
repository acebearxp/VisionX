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
    // �ָ���λ��
    const float m_fSeparator = 0.8f;

    // GDI+��Դ
    HDC m_hdcMem;
    HBITMAP m_hBmpMem;
    std::unique_ptr<Gdiplus::SolidBrush> m_uptrBrushBK;
    std::unique_ptr<Gdiplus::Pen> m_uptrPenGray;

    // ����Ĭ�ϴ���λ��
    RECT calcDefaultWindowRect();
    // ��ͼ��
    void pickImages();
    // �ر�ͼ��
    void clearImages();
    // ����ͼ���������
    void calcRectForImage(Gdiplus::Rect& rc);
private:
    // �����ź�
    HANDLE m_evWakeUp;
    // �˳���־
    std::atomic_bool m_atomExit;
    // �����߳�
    std::thread m_thread;

    // ����
    CRITICAL_SECTION m_csInput;
    // ������ͼƬ·��
    std::vector<std::wstring> m_vPaths;

    // ��̨����
    void doWork();
private:
    static const wchar_t c_wszClsName[];
    // ����ͼ����������Ա���ԭʼ����
    static void resizeRectForImage(const std::unique_ptr<Gdiplus::Bitmap>& uptrBMP, Gdiplus::Rect& rc);
};

