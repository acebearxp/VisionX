#pragma once
#include "XWnd.h"
#include "resource.h"
#include "CamCali.h"

class CMainWnd final: public CXWnd
{
public:
    CMainWnd();
    virtual ~CMainWnd();

    virtual ATOM Register(HINSTANCE hInstance);
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
    void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
    void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
    void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
private:
    static const wchar_t c_wszClsName[];
    // �ߴ�ϵ��
    const int m_nRightWidth = 200; // �����ָ���
    const int m_nMargin = 4; // �߾�
    float m_fLineHeight = -1.0f; // �и�,�������

    // GDI+��Դ
    HDC m_hdcMem;
    HBITMAP m_hBmpMem;
    std::unique_ptr<Gdiplus::Font> m_uptrFont;
    std::unique_ptr<Gdiplus::SolidBrush> m_uptrBrush;
    std::unique_ptr<Gdiplus::SolidBrush> m_uptrBrushBK;
    std::unique_ptr<Gdiplus::SolidBrush> m_uptrBrushFocus;
    std::unique_ptr<Gdiplus::Pen> m_uptrPen;

    bool m_bOpened = false; // ͼ���Ѿ��򿪱�־
    int m_nTracking = 0; // ��ǰ����������� 0���� 1~4ϵ��K

    Gdiplus::Point m_pointA, m_pointB;
    Gdiplus::Rect m_rcBtn;
    bool m_bTracking = false;
    int m_nTrackingYDelta;

    // ϵ��
    const float m_fFocusMin = 50.0f, m_fFocusMax = 1500.0f;
    float m_fFocus = 800.0f;
    const float m_fdMin = -5.0f, m_fdMax = 5.0f;
    std::vector<float> m_fd4 = {0.0f, 0.0f, 0.0f, 0.0f};

    // ��ͷ�����������
    std::unique_ptr<CamCali> m_uptrCC;

    // ��ͼ��
    void pickImage();
    // ����ͼ���������
    void calcRectForImage(Gdiplus::Rect& rc);
    // ���㽹��λ�ðٷֱ�
    float calcFocusPos();
    bool calcFocusPosBack(int y);
    // ����ϵ��Kλ�ðٷֱ�
    float calcKPos(int n);
    bool calcKPosBack(int y);
};

