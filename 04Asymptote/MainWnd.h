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
    void OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);

    void OnGetMinMaxInfo(HWND hwnd, LPMINMAXINFO lpMinMaxInfo);
private:
    // �ָ���λ��
    float m_fSeparator = 0.7f;

    // GDI+��Դ
    HDC m_hdcMem;
    HBITMAP m_hBmpMem;
    std::unique_ptr<Gdiplus::SolidBrush> m_uptrBrushBK;
    std::unique_ptr<Gdiplus::Pen> m_uptrPenGray;
    std::unique_ptr<Gdiplus::Pen> m_uptrPenCrimson;
    std::unique_ptr<Gdiplus::Pen> m_uptrPenOrange;
    std::unique_ptr<Gdiplus::Font> m_uptrFontSong;
    std::unique_ptr<Gdiplus::SolidBrush> m_uptrBrushText;

    // ����Ĭ�ϴ���λ��
    RECT calcDefaultWindowRect();
    // ��ͼ��
    void pickImages();
    // �ر�ͼ��
    void clearImages();
    // ��������theta����
    void drawCurve(Gdiplus::Graphics& g, const Gdiplus::Rect& rc);
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
    // 135��Ч����(mm)
    float m_fFocus135 = 15.0f;
    // ˮƽƫ��
    std::vector<float> m_vYaw = { 0.0f, 0.0f, 0.0f, 0.0f };
    // ������
    std::vector<float> m_vPitch = { -0.47f, -0.3f, 0.0f, 0.0f };
    // ϵ�����ڲ���
    float m_fStep = 0.1f;
    // �����ͼ��
    std::unique_ptr<CVBitmap> m_uptrOutputBitmap;
    // �����ͼ����м���
    std::vector<std::unique_ptr<CVBitmap>> m_vuptrBitmaps;

    // �ؼ��������
    RX6000 m_rx6k;

    // ��̨����
    void doWork();
    std::vector<std::wstring> doWorkForInput(float& f135, std::vector<float>& vk, std::vector<float>& vPitch, std::vector<float>& vYaw);
    void doWorkForOutput(bool bUpdateMiddle, bool bUpdateFinal);
private:
    static const wchar_t c_wszClsName[];
    // ����ͼ����������Ա���ԭʼ����
    static void resizeRectForImage(Gdiplus::Bitmap& bmp, Gdiplus::Rect& rc);
    // w2a
    static std::vector<std::string> convert(const std::vector<std::wstring>& vSrcW);
};

