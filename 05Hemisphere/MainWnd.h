#pragma once
#include "XWnd.h"
#include "resource.h"
#include "Cube.h"

struct ConstantBuffer
{
    DirectX::XMMATRIX mWorld;
    DirectX::XMMATRIX mView;
    DirectX::XMMATRIX mProjection;

    DirectX::XMFLOAT4 vLightDir;
    DirectX::XMFLOAT4 vLightColor;
};

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
    virtual void OnTimer(HWND hwnd, UINT id);

    void OnGetMinMaxInfo(HWND hwnd, LPMINMAXINFO lpMinMaxInfo);
private:
    // D3D11
    Microsoft::WRL::ComPtr<ID3D11Device> m_spD3D11;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_spImCtx;
    Microsoft::WRL::ComPtr<IDXGISwapChain1> m_spSwapChain;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_spZ;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_spZView;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_spRTV;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_spIL;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_spConstant;

    // shader
    DWORD m_dwVS, m_dwPS;
    std::unique_ptr<BYTE> m_uptrVS, m_uptrPS;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_spVS;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_spPS;

    // geometry
    std::unique_ptr<CCube> m_uptrCube;
    // constant buffer
    ConstantBuffer m_cb;
    // tick
    ULONGLONG m_u64Begin;

    // 计算默认窗口位置
    RECT calcDefaultWindowRect();
    // 加载shader
    HRESULT loadShader();
private:
    static const wchar_t c_wszClsName[];
};

