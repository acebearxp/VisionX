#pragma once
#include "Geometry.h"

class Sphere4 : public Geometry
{
public:
    Sphere4();

    void EnableTexture(int nTextured) { m_nTextured = nTextured; }

    HRESULT CreateD3DResources(Microsoft::WRL::ComPtr<ID3D11Device>& spD3D11Dev) override;
    void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spImCtx, const Space& space) override;
private:
    // ������뾶
    float m_fRadius = 24.0f;

    int m_nStepsArc = 36; // Բ�ָ��
    int m_nTextured = 1;

    // ����
    std::vector<ColorPoint> m_vSideVertices;
    std::vector<UINT> m_vSideIndexes;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_spSideVertices;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_spSideIndexes;

    // texture
    std::vector<Texture2DResource> m_vTex2D;
    std::vector<DirectX::XMFLOAT3> m_vCamPos;
    std::vector<float> m_vPitch;

    // state
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_spDepthStencilState;

    void init();
};

