#include "stdafx.h"
#include "Cylinder.h"

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

Cylinder::Cylinder()
{
	init();
}

HRESULT Cylinder::CreateD3DResources(Microsoft::WRL::ComPtr<ID3D11Device>& spD3D11Dev)
{
    HRESULT hr = Geometry::CreateD3DResources(spD3D11Dev);
    if (FAILED(hr)) return hr;

    auto fnCreateD3DBuf = [&spD3D11Dev](const vector<ColorPoint>& vVertices, const vector<UINT>& vIndexes,
        ComPtr<ID3D11Buffer>& spVertices, ComPtr<ID3D11Buffer>& spIndexes)->HRESULT
    {
        // 顶点缓冲区
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

    hr = fnCreateD3DBuf(m_vBottomVertices, m_vBottomIndexes, m_spBottomVertices, m_spBottomIndexes);
    if (FAILED(hr)) return hr;

    hr = fnCreateD3DBuf(m_vSideVertices, m_vSideIndexes, m_spSideVertices, m_spSideIndexes);
    if (FAILED(hr)) return hr;

    hr = loadTexture2D(spD3D11Dev, R"(D:/VisionX/01Hello/1.jpg)");
    if (FAILED(hr)) return hr;

    return S_OK;
}

void Cylinder::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spImCtx, const Space& space)
{
    // shader
    spImCtx->VSSetShader(m_spVS.Get(), NULL, 0);
    spImCtx->PSSetShader(m_spPS.Get(), NULL, 0);

    // constant buffer
    ConstantBuffer constBuf;
    constBuf.mWorldViewProjection = XMMatrixTranspose(space.mWorld * space.mView * space.mProjection);
    constBuf.nTextured = 1;

    spImCtx->UpdateSubresource(m_spConstBuf.Get(), 0, nullptr, &constBuf, 0, 0);
    spImCtx->VSSetConstantBuffers(0, 1, m_spConstBuf.GetAddressOf());
    spImCtx->PSSetConstantBuffers(0, 1, m_spConstBuf.GetAddressOf());

    spImCtx->IASetInputLayout(m_spIL.Get());

    // bottom
    UINT stride = sizeof(ColorPoint);
    UINT offset = 0;
    spImCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    spImCtx->IASetVertexBuffers(0, 1, m_spBottomVertices.GetAddressOf(), &stride, &offset);
    spImCtx->IASetIndexBuffer(m_spBottomIndexes.Get(), DXGI_FORMAT_R32_UINT, 0);
    spImCtx->DrawIndexed(static_cast<UINT>(m_vBottomIndexes.size()), 0, 0);

    // side
    offset = 0;
    spImCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    spImCtx->IASetVertexBuffers(0, 1, m_spSideVertices.GetAddressOf(), &stride, &offset);
    spImCtx->IASetIndexBuffer(m_spSideIndexes.Get(), DXGI_FORMAT_R32_UINT, 0);
    spImCtx->Draw(static_cast<UINT>(m_vSideVertices.size()), 0);

    // texture
    spImCtx->PSSetSamplers(0, 1, m_spSamplerState.GetAddressOf());
    spImCtx->PSSetShaderResources(0, 1, m_spSRV.GetAddressOf());
}

void Cylinder::init()
{
    XMFLOAT4 xmf4Color = XMFLOAT4(0.8f, 0.6f, 0.0f, 1.0f);
    // center point of bottom
    m_vBottomVertices.push_back({ XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) });

    float fy0 = 0.0f, fy1 = m_fHeight;
    XMFLOAT4 xmf4Up(0.0f, 1.0f, 0.0f, 1.0f);
	for (int i = 0; i <= m_nStepsArc; i++) {
		float fTheta = 2.0f * XM_PI * i / m_nStepsArc; // 方位角
        float fx1 = XMScalarSin(fTheta);
        float fz1 = XMScalarCos(fTheta);;
		float fx = m_fRadius * fx1;
		float fz = m_fRadius * fz1;

        // clamp
        float fU = (fTheta - XM_PIDIV4) / XM_PIDIV2;
        

		m_vBottomVertices.push_back({ XMFLOAT4(fx, 0.0f, fz, 1.0f), xmf4Color, xmf4Up });

        XMFLOAT4 xmf4ToCenter(-fx1, 0.0f, -fz1, 1.0f); // 指向柱心
        m_vSideVertices.push_back({ XMFLOAT4(fx, fy0, fz, 1.0f), xmf4Color, xmf4ToCenter });
        m_vSideVertices.push_back({ XMFLOAT4(fx, fy1, fz, 1.0f), xmf4Color, xmf4ToCenter });
        if (i == 0) continue;
        // triangle list for bottom
        m_vBottomIndexes.push_back(0);
        m_vBottomIndexes.push_back(i);
        m_vBottomIndexes.push_back(i + 1);
	}
}