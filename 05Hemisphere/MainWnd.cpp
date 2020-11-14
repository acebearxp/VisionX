#include "stdafx.h"
#include "MainWnd.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

const wchar_t CMainWnd::c_wszClsName[] = L"CMainWnd";

ATOM CMainWnd::Register(HINSTANCE hInstance)
{
    WNDCLASS wcls;
    ZeroMemory(&wcls, sizeof(WNDCLASS));
    wcls.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcls.lpfnWndProc = CXWnd::WindowProc;
    wcls.hInstance = hInstance;
    wcls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wcls.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcls.lpszClassName = c_wszClsName;
    wcls.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_APP));
    wcls.lpszMenuName = MAKEINTRESOURCE(IDR_MENU_MAIN);

    return RegisterClass(&wcls);
}

BOOL CMainWnd::Create(HINSTANCE hInstance)
{
    RECT rc = calcDefaultWindowRect();

    WNDCLASS cls;
    if (!GetClassInfo(hInstance, c_wszClsName, &cls)) Register(hInstance);

    const wstring wstrTitle(L"Hemisphere");
    m_hwnd = CreateWindow(c_wszClsName, wstrTitle.c_str(), WS_OVERLAPPEDWINDOW|WS_VISIBLE,
        rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
        NULL, NULL, hInstance, this);
    if (!m_hwnd) {
        MessageBox(NULL, L"Create window failed", L"Error", MB_OK);
        return FALSE;
    }
    else {
        BOOL enable;
        DwmIsCompositionEnabled(&enable);
        if (enable) {
            DWMNCRENDERINGPOLICY policy = DWMNCRP_ENABLED;
            DwmSetWindowAttribute(m_hwnd, DWMWA_NCRENDERING_POLICY, &policy, sizeof(DWMNCRENDERINGPOLICY));
        }
    }

    return TRUE;
}

void CMainWnd::Render()
{
    // clear
    m_spImCtx->ClearRenderTargetView(m_spRTV.Get(), Colors::Black);
    m_spImCtx->ClearDepthStencilView(m_spZView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    // set render target
    m_spImCtx->OMSetRenderTargets(1, m_spRTV.GetAddressOf(), m_spZView.Get());

    // rotate
    // m_cb.mWorld = XMMatrixRotationY(0.0f);
    m_cb.mWorld = XMMatrixRotationY((GetTickCount64() - m_u64Begin) / 3000.0f);
    

    ConstantBuffer cb1;
    cb1.mWorld = XMMatrixTranspose(m_cb.mWorld);
    cb1.mView = XMMatrixTranspose(m_cb.mView);
    cb1.mProjection = XMMatrixTranspose(m_cb.mProjection);
    cb1.vLightDir = m_cb.vLightDir;
    cb1.vLightColor = m_cb.vLightColor;
    m_spImCtx->UpdateSubresource(m_spConstant.Get(), 0, nullptr, &cb1, 0, 0);
    m_spImCtx->VSSetConstantBuffers(0, 1, m_spConstant.GetAddressOf());
    m_spImCtx->PSSetConstantBuffers(0, 1, m_spConstant.GetAddressOf());
    m_uptrCylinder->Draw(m_spImCtx);

    cb1.mWorld = XMMatrixTranspose(XMMatrixMultiply(XMMatrixTranslation(0.0f, 1.0f, 0.0f), m_cb.mWorld));
    m_spImCtx->UpdateSubresource(m_spConstant.Get(), 0, nullptr, &cb1, 0, 0);
    m_spImCtx->VSSetConstantBuffers(0, 1, m_spConstant.GetAddressOf());
    m_spImCtx->PSSetConstantBuffers(0, 1, m_spConstant.GetAddressOf());
    m_uptrCube->Draw(m_spImCtx);

    HRESULT hr = m_spSwapChain->Present(1, 0);
}

LRESULT CMainWnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRet;
    switch (uMsg) {
        HANDLE_MSG(hWnd, WM_ERASEBKGND, OnEraseBkgnd);
        HANDLE_MSG(hWnd, WM_SIZE, OnSize);
        HANDLE_MSG(hWnd, WM_GETMINMAXINFO, OnGetMinMaxInfo);
    default:
        lRet = CXWnd::WndProc(hWnd, uMsg, wParam, lParam);
        break;
    }

    return lRet;
}

BOOL CMainWnd::OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    BOOL bRet = CXWnd::OnCreate(hwnd, lpCreateStruct);
    if (!bRet) return bRet;

    // 创建D3D设备
    UINT uFlagDXGI = 0, uFlag = 0;
#ifdef _DEBUG
    uFlagDXGI |= DXGI_CREATE_FACTORY_DEBUG;
    uFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    ComPtr<IDXGIFactory3> spFactory;
    HRESULT hr = CreateDXGIFactory2(uFlagDXGI, IID_PPV_ARGS(&spFactory));
    if (FAILED(hr)) {
        MessageBox(hwnd, L"Create DXGI Factory Failed!", L"D3D11", MB_OK);
        return FALSE;
    }

    hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, uFlag, NULL, 0, D3D11_SDK_VERSION, &m_spD3D11, NULL, &m_spImCtx);
    if (FAILED(hr)) {
        MessageBox(hwnd, L"Create Device Failed!", L"D3D11", MB_OK);
        return FALSE;
    }

    // 创建交换链
    DXGI_SWAP_CHAIN_DESC1 desc;
    ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC1));
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.Stereo = FALSE;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 2;
    desc.Scaling = DXGI_SCALING_STRETCH;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    hr = spFactory->CreateSwapChainForHwnd(m_spD3D11.Get(), hwnd, &desc, NULL, NULL, &m_spSwapChain);
    if (FAILED(hr)) {
        MessageBox(hwnd, L"Create Swap Chain Failed!", L"D3D11", MB_OK);
        return FALSE;
    }

    // 光栅参数
    D3D11_RASTERIZER_DESC descRS;
    ZeroMemory(&descRS, sizeof(D3D11_RASTERIZER_DESC));
    descRS.FillMode = D3D11_FILL_WIREFRAME; // D3D11_FILL_SOLID
    descRS.CullMode = D3D11_CULL_BACK;      // D3D11_CULL_NONE
    descRS.DepthClipEnable = TRUE;
    descRS.MultisampleEnable = TRUE;

    ComPtr<ID3D11RasterizerState> spRS;
    hr = m_spD3D11->CreateRasterizerState(&descRS, &spRS);
    m_spImCtx->RSSetState(spRS.Get());

    // 几何体
    m_uptrCube = unique_ptr<Cube>(new Cube());
    m_uptrCube->CreateD3DBuf(m_spD3D11);

    m_uptrCylinder = unique_ptr<Cylinder>(new Cylinder());
    m_uptrCylinder->CreateD3DBuf(m_spD3D11);

    // 3D空间
    D3D11_BUFFER_DESC descWorld;
    ZeroMemory(&descWorld, sizeof(D3D11_BUFFER_DESC));
    descWorld.Usage = D3D11_USAGE_DEFAULT;
    descWorld.ByteWidth = sizeof(ConstantBuffer);
    descWorld.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = m_spD3D11->CreateBuffer(&descWorld, NULL, &m_spConstant);

    // 标准空间
    m_cb.mWorld = XMMatrixIdentity();

    // 摄像机位
    XMVECTOR Eye = XMVectorSet(3.0f, 8.0f, -30.0f, 0.0f);
    // XMVECTOR Eye = XMVectorSet(0.0f, 8.0f, -10.0f, 0.0f);
    // XMVECTOR Eye = XMVectorSet(30.0f, 30.0f, -80.0f, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f, 6.0f, 0.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    m_cb.mView = XMMatrixLookAtLH(Eye, At, Up);

    // 灯光
    m_cb.vLightDir = XMFLOAT4(-0.577f, 0.577f, -0.577f, 1.0f);
    m_cb.vLightColor = XMFLOAT4(0.5f, 0.5f, 0.3f, 1.0f);

    OnSize(hwnd, 0, lpCreateStruct->cx, lpCreateStruct->cy);

    // 开始时间标定
    m_u64Begin = GetTickCount64();

    return bRet;
}

void CMainWnd::OnDestroy(HWND hwnd)
{
    CXWnd::OnDestroy(hwnd);

    PostQuitMessage(0);
}

void CMainWnd::OnPaint(HWND hwnd)
{
    CXWnd::OnPaint(hwnd);
    Render();
}

BOOL CMainWnd::OnEraseBkgnd(HWND hwnd, HDC hdc)
{
    return TRUE;
}

void CMainWnd::OnSize(HWND hwnd, UINT state, int cx, int cy)
{
    // 释放先前的
    m_spRTV.Reset();
    m_spZView.Reset();
    m_spZ.Reset();

    // 尺寸不能是0
    if (cx == 0) cx = 1;
    if (cy == 0) cy = 1;

    // resize
    HRESULT hr = m_spSwapChain->ResizeBuffers(0, cx, cy, DXGI_FORMAT_UNKNOWN, 0);
    // 设定渲染目标
    ComPtr<ID3D11Texture2D> spBackBuffer;
    hr = m_spSwapChain->GetBuffer(0, IID_PPV_ARGS(&spBackBuffer));
    hr = m_spD3D11->CreateRenderTargetView(spBackBuffer.Get(), NULL, &m_spRTV);

    // Z-Buffer
    D3D11_TEXTURE2D_DESC descZ;
    ZeroMemory(&descZ, sizeof(D3D11_TEXTURE2D_DESC));
    descZ.Width = cx;
    descZ.Height = cy;
    descZ.MipLevels = 1;
    descZ.ArraySize = 1;
    descZ.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descZ.SampleDesc.Count = 1;
    descZ.SampleDesc.Quality = 0;
    descZ.Usage = D3D11_USAGE_DEFAULT;
    descZ.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    hr = m_spD3D11->CreateTexture2D(&descZ, NULL, &m_spZ);

    D3D11_DEPTH_STENCIL_VIEW_DESC descZView;
    ZeroMemory(&descZView, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    descZView.Format = descZ.Format;
    descZView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    hr = m_spD3D11->CreateDepthStencilView(m_spZ.Get(), &descZView, &m_spZView);

    // 设定可见区域
    D3D11_VIEWPORT viewPort;
    viewPort.TopLeftX = 0.0f;
    viewPort.TopLeftY = 0.0f;
    viewPort.Width = static_cast<float>(cx);
    viewPort.Height = static_cast<float>(cy);
    viewPort.MinDepth = 0.0f;
    viewPort.MaxDepth = 1.0f;
    m_spImCtx->RSSetViewports(1, &viewPort);

    // 镜头
    m_cb.mProjection = XMMatrixPerspectiveFovLH(XM_PI / 6.0f, viewPort.Width / viewPort.Height, 0.01f, 1000.0f);
}

void CMainWnd::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case ID_FILE_OPEN:
        break;
    case ID_FILE_EXIT:
        SendMessage(hwnd, WM_CLOSE, 0, 0);
        break;
    default:
        CXWnd::OnCommand(hwnd, id, hwndCtl, codeNotify);
        break;
    }
}

void CMainWnd::OnGetMinMaxInfo(HWND hwnd, LPMINMAXINFO lpMinMaxInfo)
{
    lpMinMaxInfo->ptMinTrackSize.x = 400;
    lpMinMaxInfo->ptMinTrackSize.y = 300;
}

RECT CMainWnd::calcDefaultWindowRect()
{
    RECT rc;

    // 枚举显示器,默认显示在较大的显示器上
    EnumDisplayMonitors(NULL, nullptr,
        [](HMONITOR hMonitor, HDC hdc, LPRECT pRECT, LPARAM pParam)->BOOL
    {
        RECT& rc = *reinterpret_cast<RECT*>(pParam);
        float fArea = 1.0f * (rc.right - rc.left) * (rc.bottom - rc.top);
        if (1.0f * (pRECT->right - pRECT->left) * (pRECT->bottom - pRECT->top) > fArea) {
            rc = *pRECT;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&rc));

    // 如果前面的办法没有获取到,使用默认显示器参数
    if (rc.right - rc.left <= 0 || rc.bottom - rc.top <= 0) {
        rc.left = rc.top = 0;
        rc.right = GetSystemMetrics(SM_CXSCREEN);
        rc.bottom = GetSystemMetrics(SM_CYSCREEN);
    }

    // 最大不超过800x600
    const int maxWidth = 800, maxHeight = 600;
    auto fnAdjust = [](LONG& min, LONG& max, int limit) {
        if (max - min > limit) {
            int center = (min + max) / 2;
            min = center - limit / 2;
            max = center + limit / 2;
        }
    };
    fnAdjust(rc.left, rc.right, maxWidth);
    fnAdjust(rc.top, rc.bottom, maxHeight);

    return rc;
}

HRESULT CMainWnd::loadShader()
{
    const int size = MAX_PATH;
    wchar_t buf[size];
    GetModuleFileName(NULL, buf, size);
    wchar_t* pLast = wcsrchr(buf, L'\\');
    if (pLast == NULL) return E_FAIL;

    auto fnLoad = [](const wchar_t* path, unique_ptr<BYTE>& uptrShader, DWORD& dwSize)->HRESULT{
        HANDLE hCSO = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hCSO == INVALID_HANDLE_VALUE) return E_HANDLE;
        dwSize = GetFileSize(hCSO, NULL);
        uptrShader = unique_ptr<BYTE>(new BYTE[dwSize]);
        ReadFile(hCSO, uptrShader.get(), dwSize, &dwSize, NULL);
        CloseHandle(hCSO);
        return S_OK;
    };

    wcscpy_s(pLast + 1, size - ((pLast + 1) - buf), L"VS.cso");
    fnLoad(buf, m_uptrVS, m_dwVS);
    HRESULT hr = m_spD3D11->CreateVertexShader(m_uptrVS.get(), m_dwVS, NULL, &m_spVS);
    if (FAILED(hr)) return hr;

    wcscpy_s(pLast + 1, size - ((pLast + 1) - buf), L"PS.cso");
    fnLoad(buf, m_uptrPS, m_dwPS);
    hr = m_spD3D11->CreatePixelShader(m_uptrPS.get(), m_dwPS, NULL, &m_spPS);
    if (FAILED(hr)) return hr;

    return S_OK;
}
