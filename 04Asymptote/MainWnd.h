#pragma once
#include "XWnd.h"
#include "resource.h"
#include "RX6000.h"
#include "CVBitmap.h"

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
    float m_fSeparator = 0.7f;

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
    CRITICAL_SECTION m_csRX6K;
    // ������ͼƬ·��
    std::vector<std::wstring> m_vPaths;
    // ����ϵ��
    std::vector<float> m_vk = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    // �����ͼ��
    std::unique_ptr<CVBitmap> m_uptrOutputBitmap;
    // �����ͼ����м���
    std::vector<std::unique_ptr<CVBitmap>> m_vuptrBitmaps;

    // �ؼ��������
    RX6000 m_rx6k;

    // ��̨����
    void doWork();
    std::vector<std::wstring> doWorkForInput(std::vector<float>& vk);
    void doWorkForOutput(bool bUpdateMiddle, bool bUpdateFinal);
private:
    static const wchar_t c_wszClsName[];
    // ����ͼ����������Ա���ԭʼ����
    static void resizeRectForImage(Gdiplus::Bitmap& bmp, Gdiplus::Rect& rc);
    // w2a
    static std::vector<std::string> convert(const std::vector<std::wstring>& vSrcW);
};

