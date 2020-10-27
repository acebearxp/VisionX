#pragma once
#include "XWnd.h"
#include "resource.h"
#include "Mate40.h"

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
    static const wchar_t c_wszClsName[];
    // GDI+��Դ
    HDC m_hdcMem;
    HBITMAP m_hBmpMem;
    unique_ptr<Gdiplus::SolidBrush> m_uptrBrushBK;
    unique_ptr<Gdiplus::Pen> m_uptrPen;

    // �ؼ��������
    Mate40 m_mate40;

    // ��̨�����߳�
    thread m_thread;
    // �ź��¼�
    HANDLE m_evPuls;
    // �˳���־
    atomic_bool m_atomQuit;
    // �����־
    atomic_bool m_atomJob;

    // ������
    CRITICAL_SECTION m_cs;
    // �ļ�·��
    vector<wstring> m_vImagePaths;

    // ��ͼ��
    void pickImages();
    // �ر�ͼ��
    void clearImages();
    // �����߳�
    void doWork();
    // wstring to string
    vector<string> convert(const vector<wstring>& vSrc);
private:
    // ����ͼ����������Ա���ԭʼ����
    static void resizeRectForImage(const unique_ptr<Gdiplus::Bitmap>& uptrBMP, Gdiplus::Rect& rc);
};

