#pragma once
#include "Geometry.h"

class Sphere4 : public Geometry
{
public:
    Sphere4();

    HRESULT CreateD3DResources(Microsoft::WRL::ComPtr<ID3D11Device>& spD3D11Dev) override;
    void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spImCtx, const Space& space) override;
private:
    // 侧面球半径
    float m_fRadius = 20.0f;

    int m_nStepsArc = 36; // 圆分割步数

    // 侧面
    std::vector<ColorPoint> m_vSideVertices;
    std::vector<UINT> m_vSideIndexes;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_spSideVertices;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_spSideIndexes;

    // texture
    std::vector<Texture2DResource> m_vTex2D;
    std::vector<DirectX::XMFLOAT3> m_vCamPos;
    std::vector<float> m_vPitch;

    void init();
};

