﻿#pragma once
#include "XWnd.h"
#include "resource.h"
#include "Cube.h"
#include "Cylinder.h"
#include "Arc4.h"

class CMainWnd final : public CXWnd
{
public:
    CMainWnd() {};
    ~CMainWnd() override {};

    ATOM Register(HINSTANCE hInstance) override;
    BOOL Create(HINSTANCE hInstance);

    void Render();

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
    // D3D11
    Microsoft::WRL::ComPtr<ID3D11Device> m_spD3D11;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_spImCtx;
    
    Microsoft::WRL::ComPtr<IDXGISwapChain1> m_spSwapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_spRTV;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_spZ;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_spZView;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_spRSSolid;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_spRSWireframe;

    // geometry
    std::unique_ptr<Cube> m_uptrCube;
    std::unique_ptr<Cylinder> m_uptrCylinder;
    std::unique_ptr<Arc4> m_uptrArc4;

    // world view projection
    Space m_space;

    // tick
    ULONGLONG m_u64Begin;

    // 计算默认窗口位置
    RECT calcDefaultWindowRect();
private:
    static const wchar_t c_wszClsName[];
};
