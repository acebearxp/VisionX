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
    // �ߴ�ϵ��
    const int m_nRightWidth = 200; // �����ָ���
    const int m_nMargin = 4; // �߾�
    float m_fLineHeight = -1.0f; // �и�,�������

    // GDI+��Դ
    std::unique_ptr<Gdiplus::Font> m_uptrFont;
    std::unique_ptr<Gdiplus::SolidBrush> m_uptrBrush;
    std::unique_ptr<Gdiplus::SolidBrush> m_uptrBrushFocus;
    std::unique_ptr<Gdiplus::Pen> m_uptrPen;

    bool m_bOpened = false; // ͼ���Ѿ��򿪱�־
    int m_nTracking = 0; // ��ǰ����������� 0���� 1~4ϵ��K

    Gdiplus::Point m_pointA, m_pointB;
    Gdiplus::Rect m_rcBtn;
    bool m_bTracking = false;
    int m_nTrackingYDelta;

    // ϵ��
    float m_fFocus = 500.0f;
    std::vector<float> m_fK4 = {2.0f, 3.0f, 1.0f, 0.0f};

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

