#include "stdafx.h"
#include "Cylinder.h"

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

Cylinder::Cylinder()
{
	init();
}

HRESULT Cylinder::CreateD3DBuf(Microsoft::WRL::ComPtr<ID3D11Device>& spD3D11Dev)
{
    HRESULT hr = Geometry::CreateD3DBuf(spD3D11Dev);
    if (FAILED(hr)) return hr;

    auto fnCreateD3DBuf = [&spD3D11Dev](const vector<ColorPoint>& vVertexes, const vector<UINT>& vIndexes,
        ComPtr<ID3D11Buffer>& spVertexes, ComPtr<ID3D11Buffer>& spIndexes)->HRESULT
    {
        // 顶点缓冲区
        D3D11_BUFFER_DESC descVertex;
        ZeroMemory(&descVertex, sizeof(D3D11_BUFFER_DESC));
        descVertex.ByteWidth = sizeof(ColorPoint) * static_cast<UINT>(vVertexes.size());
        descVertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA xinit;;
        ZeroMemory(&xinit, sizeof(D3D11_SUBRESOURCE_DATA));
        xinit.pSysMem = vVertexes.data();
        xinit.SysMemPitch = 0;
        xinit.SysMemSlicePitch = 0;

        HRESULT hr = spD3D11Dev->CreateBuffer(&descVertex, &xinit, &spVertexes);
        if (FAILED(hr)) return hr;

        // 索引缓冲区
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

    hr = fnCreateD3DBuf(m_vBottomVertexes, m_vBottomIndexes, m_spBottomVertexes, m_spBottomIndexes);
    if (FAILED(hr)) return hr;

    hr = fnCreateD3DBuf(m_vSideVertexes, m_vSideIndexes, m_spSideVertexes, m_spSideIndexes);
    if (FAILED(hr)) return hr;

    return S_OK;
}

void Cylinder::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spImCtx)
{
    // shader
    spImCtx->VSSetShader(m_spVS.Get(), NULL, 0);
    spImCtx->PSSetShader(m_spPS.Get(), NULL, 0);

    spImCtx->IASetInputLayout(m_spIL.Get());

    // bottom
    UINT stride = sizeof(ColorPoint);
    UINT offset = 0;
    spImCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    spImCtx->IASetVertexBuffers(0, 1, m_spBottomVertexes.GetAddressOf(), &stride, &offset);
    spImCtx->IASetIndexBuffer(m_spBottomIndexes.Get(), DXGI_FORMAT_R32_UINT, 0);
    spImCtx->DrawIndexed(static_cast<UINT>(m_vBottomIndexes.size()), 0, 0);

    // side
    offset = 0;
    spImCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    spImCtx->IASetVertexBuffers(0, 1, m_spSideVertexes.GetAddressOf(), &stride, &offset);
    spImCtx->IASetIndexBuffer(m_spSideIndexes.Get(), DXGI_FORMAT_R32_UINT, 0);
    spImCtx->Draw(static_cast<UINT>(m_vSideVertexes.size()), 0);
}

void Cylinder::init()
{
    // center point of bottom
    m_vBottomVertexes.push_back({ XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) });

    float fy0 = 0.0f, fy1 = m_fHeight;
	for (int i = 0; i <= m_nStepsArc; i++) {
		float fTheta = 2.0f * XM_PI * i / m_nStepsArc;
		float fx = m_fRadius * XMScalarSin(fTheta);
		float fz = m_fRadius * XMScalarCos(fTheta);
		m_vBottomVertexes.push_back({ XMFLOAT4(fx, 0.0f, fz, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) });
        m_vSideVertexes.push_back({ XMFLOAT4(fx, fy0, fz, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) });
        m_vSideVertexes.push_back({ XMFLOAT4(fx, fy1, fz, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) });
        if (i == 0) continue;
        // triangle list for bottom
        m_vBottomIndexes.push_back(0);
        m_vBottomIndexes.push_back(i);
        m_vBottomIndexes.push_back(i + 1);
	}
}