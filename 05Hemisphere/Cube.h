#pragma once

#include "geometry.h"

class Cube : public Geometry
{
public:
    Cube();

    HRESULT CreateD3DBuf(Microsoft::WRL::ComPtr<ID3D11Device>& spD3D11Dev) override;
    void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spImCtx) override;
private:
    std::vector<ColorPoint> m_vVertexes;
    std::vector<UINT> m_vIndexes;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_spVertexes;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_spIndexes;

    // 初始化数据
    void init();
};

