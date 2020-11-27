#include "stdafx.h"
#include "Sphere4.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

Sphere4::Sphere4()
{
	init();
}

HRESULT Sphere4::CreateD3DResources(ComPtr<ID3D11Device>& spD3D11Dev)
{
    HRESULT hr = Geometry::CreateD3DResources(spD3D11Dev);
    if (FAILED(hr)) return hr;

    auto fnCreateD3DBuf = [&spD3D11Dev](const vector<ColorPoint>& vVertices, const vector<UINT>& vIndexes,
        ComPtr<ID3D11Buffer>& spVertices, ComPtr<ID3D11Buffer>& spIndexes)->HRESULT
    {
        // ���㻺����
        D3D11_BUFFER_DESC descVertex;
        ZeroMemory(&descVertex, sizeof(D3D11_BUFFER_DESC));
        descVertex.ByteWidth = sizeof(ColorPoint) * static_cast<UINT>(vVertices.size());
        descVertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA xinit;;
        ZeroMemory(&xinit, sizeof(D3D11_SUBRESOURCE_DATA));
        xinit.pSysMem = vVertices.data();
        xinit.SysMemPitch = 0;
        xinit.SysMemSlicePitch = 0;

        HRESULT hr = spD3D11Dev->CreateBuffer(&descVertex, &xinit, &spVertices);
        if (FAILED(hr)) return hr;

        // ����������
        if (vIndexes.size() > 0) {
            D3D11_BUFFER_DESC descCubeIndex;
            ZeroMemory(&descCubeIndex, sizeof(D3D11_BUFFER_DESC));
            descCubeIndex.ByteWidth = static_cast<UINT>(sizeof(UINT) * vIndexes.size());
            descCubeIndex.BindFlags = D3D11_BIND_INDEX_BUFFER;

            xinit.pSysMem = vIndexes.data();

            hr = spD3D11Dev->CreateBuffer(&descCubeIndex, &xinit, &spIndexes);
            if (FAILED(hr)) return hr;
        }

        return S_OK;
    };

    hr = fnCreateD3DBuf(m_vSideVertices, m_vSideIndexes, m_spSideVertices, m_spSideIndexes);
    if (FAILED(hr)) return hr;

    // texture
    vector<string> vPaths = {
        R"(D:/VisionX/01Hello/front.jpg)", // Pi*0/2
        R"(D:/VisionX/01Hello/right.jpg)", // Pi*1/2
        R"(D:/VisionX/01Hello/back.jpg)",  // Pi*2/2
        R"(D:/VisionX/01Hello/left.jpg)"   // Pi*3/2
    };
    for (const string& strPath : vPaths) {
        Texture2DResource tex2DRes;
        hr = loadTexture2D(spD3D11Dev, strPath, tex2DRes.spD3D11Tex2D, tex2DRes.spD3D11SRV);
        if (FAILED(hr)) return hr;

        m_vTex2D.push_back(tex2DRes);
    }

    return S_OK;
}

void Sphere4::Draw(ComPtr<ID3D11DeviceContext>& spImCtx, const Space& space)
{
    // shader
    spImCtx->VSSetShader(m_spVS.Get(), NULL, 0);
    spImCtx->PSSetShader(m_spPS.Get(), NULL, 0);

    // constant buffer
    ConstantBuffer constBuf;
    spImCtx->VSSetConstantBuffers(0, 1, m_spConstBuf.GetAddressOf());
    spImCtx->PSSetConstantBuffers(0, 1, m_spConstBuf.GetAddressOf());

    // input layout
    spImCtx->IASetInputLayout(m_spIL.Get());
    // sampler state
    spImCtx->PSSetSamplers(0, 1, m_spSamplerState.GetAddressOf());

    const UINT stride = sizeof(ColorPoint);
    const UINT offset = 0;

    vector<int> vSequence = { 0, 2, 1, 3 };
    for (int i = 0; i < 1; i++) {
        int x = vSequence[i];
        XMMATRIX mRotate = XMMatrixRotationY(XM_PIDIV2 * x);
        float fScale = (x % 2 == 0) ? 1.01f : 1.0f; // 0,2ǰ����Զ 1,3�����Խ�(for blending) 
        XMMATRIX mScale = XMMatrixScaling(fScale, 1.0f, fScale);
        constBuf.mWorldViewProjection = XMMatrixTranspose(mScale * mRotate * space.mWorld * space.mView * space.mProjection);

        // side
        constBuf.nTextured = 0;
        spImCtx->UpdateSubresource(m_spConstBuf.Get(), 0, nullptr, &constBuf, 0, 0);
        spImCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        spImCtx->IASetVertexBuffers(0, 1, m_spSideVertices.GetAddressOf(), &stride, &offset);
        spImCtx->IASetIndexBuffer(m_spSideIndexes.Get(), DXGI_FORMAT_R32_UINT, 0);
        spImCtx->DrawIndexed(static_cast<UINT>(m_vSideIndexes.size()), 0, 0);
    }
}

void Sphere4::init()
{
	XMFLOAT4 xmf4Color = XMFLOAT4(0.8f, 0.6f, 0.0f, 1.0f);
	XMFLOAT4 xmf4Up(0.0f, 1.0f, 0.0f, 1.0f);

	float fStep = 2.0f * XM_PI / m_nStepsArc;
	for (int i = 0; i < m_nStepsArc / 4; i++) {
		if (i == 0) {
			// ���ĵ�
			m_vSideVertices.push_back({ XMFLOAT4(0.0f, 0.0f, m_fRadius, 1.0f), xmf4Color, xmf4Up, XMFLOAT2(0.5f, 0.5f) });
		}
		else {
			float fPitch = fStep * i;
			float fz = m_fRadius * XMScalarCos(fPitch);
			float fr = m_fRadius * XMScalarSin(fPitch);
			for (int j = 0; j < m_nStepsArc; j++) {
				float fTheta = fStep * j;
				float fx = fr * XMScalarSin(fTheta);
				float fy = fr * XMScalarCos(fTheta);
				m_vSideVertices.push_back({ XMFLOAT4(fx, fy, fz, 1.0f), xmf4Color, xmf4Up, XMFLOAT2(0.5f, 0.5f) });
				if (i == 1) {
					// ����Ȧ
					if (j == 0) {
						m_vSideIndexes.push_back(0);
						m_vSideIndexes.push_back(m_nStepsArc);
						m_vSideIndexes.push_back(j + 1);
					}
					else {
						m_vSideIndexes.push_back(0);
						m_vSideIndexes.push_back(j);
						m_vSideIndexes.push_back(j + 1);
					}
				}
				else {
					// ��Ȧ
                    if (j == 0) {
                        m_vSideIndexes.push_back(m_nStepsArc * (i - 1));
                        m_vSideIndexes.push_back(m_nStepsArc * i);
                        m_vSideIndexes.push_back(m_nStepsArc * (i - 1) + 1);

                        m_vSideIndexes.push_back(m_nStepsArc * (i - 1));
                        m_vSideIndexes.push_back(m_nStepsArc * (i - 1) + 1);
                        m_vSideIndexes.push_back(m_nStepsArc * (i - 2) + 1);
                    }
                    else {
                        m_vSideIndexes.push_back(m_nStepsArc * (i - 2) + j);
                        m_vSideIndexes.push_back(m_nStepsArc * (i - 1) + j);
                        m_vSideIndexes.push_back(m_nStepsArc * (i - 1) + j + 1);
                        
                        m_vSideIndexes.push_back(m_nStepsArc * (i - 2) + j);
                        m_vSideIndexes.push_back(m_nStepsArc * (i - 1) + j + 1);
                        m_vSideIndexes.push_back(m_nStepsArc * (i - 2) + j + 1);
                    }
				}
			}
		}
	}
}
