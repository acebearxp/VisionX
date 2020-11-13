#pragma once

struct ColorPoint
{
    DirectX::XMFLOAT4 Point;
    DirectX::XMFLOAT4 Color;
};

class Geometry
{
public:
    virtual HRESULT CreateD3DBuf(Microsoft::WRL::ComPtr<ID3D11Device>& spD3D11Dev);
    virtual void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spImCtx) = 0;
protected:
    // shader
    DWORD m_dwVS, m_dwPS;
    std::unique_ptr<BYTE> m_uptrVS, m_uptrPS;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_spVS;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_spPS;
    // input layout
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_spIL;

    // 加载shader
    virtual HRESULT loadShader(Microsoft::WRL::ComPtr<ID3D11Device>& spD3D11Dev);
};