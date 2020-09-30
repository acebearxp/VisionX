#pragma once
#include "MainWnd.h"

class CApp
{   
public:
    CApp();
    virtual ~CApp();

    HINSTANCE GetInstance() { return m_hInstance; }

    virtual BOOL Init(HINSTANCE hInstance);
    virtual int Run(PWSTR pCmdLine, int nCmdShow);
    virtual void UnInit();
protected:
    HINSTANCE m_hInstance;
private:
    CMainWnd* m_pwndMain;
};

