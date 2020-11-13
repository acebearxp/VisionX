#include "stdafx.h"
#include "Cylinder.h"

using namespace std;
using namespace DirectX;

Cylinder::Cylinder()
{
	init();
}

HRESULT Cylinder::CreateD3DBuf(Microsoft::WRL::ComPtr<ID3D11Device>& spD3D11Dev)
{
    Geometry::CreateD3DBuf(spD3D11Dev);

    // 顶点缓冲区
    D3D11_BUFFER_DESC descVertex;
    ZeroMemory(&descVertex, sizeof(D3D11_BUFFER_DESC));
    descVertex.ByteWidth = sizeof(ColorPoint) * static_cast<UINT>(m_vBottomVertexes.size());
    descVertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA xinit;;
    ZeroMemory(&xinit, sizeof(D3D11_SUBRESOURCE_DATA));
    xinit.pSysMem = m_vBottomVertexes.data();
    xinit.SysMemPitch = 0;
    xinit.SysMemSlicePitch = 0;

    HRESULT hr = spD3D11Dev->CreateBuffer(&descVertex, &xinit, &m_spBottomVertexes);
    if (FAILED(hr)) return hr;

    // 索引缓冲区
    D3D11_BUFFER_DESC descCubeIndex;
    ZeroMemory(&descCubeIndex, sizeof(D3D11_BUFFER_DESC));
    descCubeIndex.ByteWidth = sizeof(UINT) * m_vBottomIndexes.size();
    descCubeIndex.BindFlags = D3D11_BIND_INDEX_BUFFER;

    xinit.pSysMem = m_vBottomIndexes.data();

    hr = spD3D11Dev->CreateBuffer(&descCubeIndex, &xinit, &m_spBottomIndexes);
    if (FAILED(hr)) return hr;

    return S_OK;
}

void Cylinder::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spImCtx)
{
    spImCtx->IASetInputLayout(m_spIL.Get());
    spImCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 设定顶点数据
    UINT stride = sizeof(ColorPoint);
    UINT offset = 0;
    spImCtx->IASetVertexBuffers(0, 1, m_spBottomVertexes.GetAddressOf(), &stride, &offset);
    spImCtx->IASetIndexBuffer(m_spBottomIndexes.Get(), DXGI_FORMAT_R32_UINT, 0);

    spImCtx->DrawIndexed(static_cast<UINT>(m_vBottomIndexes.size()), 0, 0);
}

void Cylinder::init()
{
	// bottom, triangle list
	m_vBottomVertexes.push_back({ XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) });

	for (int i = 0; i <= m_nStepsArc; i++) {
		float fTheta = 2.0f * XM_PI * i / m_nStepsArc;
		float fx = m_fRadius * XMScalarSin(fTheta);
		float fz = m_fRadius * XMScalarCos(fTheta);
		m_vBottomVertexes.push_back({ XMFLOAT4(fx, 0.0f, fz, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) });
        if (i > 0) {
            m_vBottomIndexes.push_back(0);
            m_vBottomIndexes.push_back(i + 1);
            m_vBottomIndexes.push_back(i);
        }
	}

	// TODO: side, triangle strip
}