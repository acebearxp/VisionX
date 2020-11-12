#pragma once

struct ColorPoint
{
    DirectX::XMFLOAT4 Point;
    DirectX::XMFLOAT4 Color;
};

class CCube
{
public:
    CCube();

    HRESULT Init(Microsoft::WRL::ComPtr<ID3D11Device>& spD3D11);
    Microsoft::WRL::ComPtr<ID3D11Buffer>& GetVertexes() { return m_spVertex; }
    Microsoft::WRL::ComPtr<ID3D11Buffer>& GetIndex() { return m_spCubeIndex; }
    UINT GetStride() { return sizeof(ColorPoint); }
    UINT GetVertexesCount() { return static_cast<UINT>(m_vCubeIndex.size()); }
private:
    std::vector<ColorPoint> m_vVertexes;
    std::vector<UINT> m_vCubeIndex;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_spVertex;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_spCubeIndex;
};

