#pragma once

#include "geometry.h"

class CCube
{
public:
    CCube();

    HRESULT CreateD3DBuf(Microsoft::WRL::ComPtr<ID3D11Device>& spD3D11);
    Microsoft::WRL::ComPtr<ID3D11Buffer>& GetVertexes() { return m_spVertex; }
    Microsoft::WRL::ComPtr<ID3D11Buffer>& GetIndex() { return m_spCubeIndex; }
    UINT GetStride() { return sizeof(ColorPoint); }
    UINT GetVertexesCount() { return static_cast<UINT>(m_vIndexes.size()); }
private:
    std::vector<ColorPoint> m_vVertexes;
    std::vector<UINT> m_vIndexes;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_spVertex;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_spCubeIndex;
};

