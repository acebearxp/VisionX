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
    void OnPaint(HWND hwnd) override;
    void OnSize(HWND hwnd, UINT state, int cx, int cy) override;
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) override;
private:
    std::unique_ptr<CamCali> m_uptrCC;
    std::thread m_threadWorking;

    void pickImage();
};

