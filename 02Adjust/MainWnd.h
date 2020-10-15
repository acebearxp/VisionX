#pragma once
#include "XWnd.h"
#include "resource.h"

class CMainWnd : public CXWnd
{
public:
    CMainWnd(CApp *pApp);
    virtual ~CMainWnd();

    virtual ATOM Register();
    BOOL Create();
protected:
    virtual void OnSize(HWND hwnd, UINT state, int cx, int cy);
    virtual void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
private:
    void PickImage();
};

