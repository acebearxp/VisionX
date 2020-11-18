#pragma once

#include "Karmeliet.h"

struct Space
{
    DirectX::XMMATRIX mWorld;
    DirectX::XMMATRIX mView;
    DirectX::XMMATRIX mProjection;
};

struct ConstantBuffer
{
    DirectX::XMMATRIX mWorldViewProjection;
    uint nTextured;
};

struct ColorPoint
{
    DirectX::XMFLOAT4 Point;
    DirectX::XMFLOAT4 Color;
    DirectX::XMFLOAT4 Normal;
    DirectX::XMFLOAT2 Tex;
};

class Geometry
{
public:
    virtual HRESULT CreateD3DResources(Microsoft::WRL::ComPtr<ID3D11Device>& spD3D11Dev);
    virtual void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spImCtx, const Space& space) = 0;
protected:
    // shader
    DWORD m_dwVS, m_dwPS;
    std::unique_ptr<BYTE> m_uptrVS, m_uptrPS;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_spVS;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_spPS;

    // constant buffer
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_spConstBuf;

    // input layout
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_spIL;
    // texture
    Karmeliet m_image;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_spSamplerState;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_spTexture2D;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_spSRV;

    // 加载shader
    virtual HRESULT loadShader(Microsoft::WRL::ComPtr<ID3D11Device>& spD3D11Dev);
    // create constant buffer
    virtual HRESULT createConstantBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& spD3D11Dev);
    // create input layout
    virtual HRESULT createInputLayout(Microsoft::WRL::ComPtr<ID3D11Device>& spD3D11Dev);
    // create sampler state
    virtual HRESULT createSamplerState(Microsoft::WRL::ComPtr<ID3D11Device>& spD3D11Dev);
    // load texture
    virtual HRESULT loadTexture2D(Microsoft::WRL::ComPtr<ID3D11Device>& spD3D11Dev, const std::string& strPathImage);
};