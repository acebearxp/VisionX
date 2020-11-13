#pragma once

#include "geometry.h"

class Cylinder : public Geometry
{
public:
    Cylinder();

    HRESULT CreateD3DBuf(Microsoft::WRL::ComPtr<ID3D11Device>& spD3D11Dev) override;
    void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spImCtx) override;
private:
    float m_fRadius = 20.0f;
    float m_fHeight = 10.0f;

    int m_nStepsArc    = 36; // 圆弧分割步数
    // 底面
    std::vector<ColorPoint> m_vBottomVertexes;
    std::vector<UINT> m_vBottomIndexes;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_spBottomVertexes;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_spBottomIndexes;

    // 侧面
    std::vector<ColorPoint> m_vSideVertexes;
    std::vector<UINT> m_vSideIndexes;

    // 初始化数据
    void init();
};
