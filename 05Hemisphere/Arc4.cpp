#include "stdafx.h"
#include "Arc4.h"

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

Arc4::Arc4()
{
    init();
}

HRESULT Arc4::CreateD3DResources(Microsoft::WRL::ComPtr<ID3D11Device>& spD3D11Dev)
{
    HRESULT hr = Geometry::CreateD3DResources(spD3D11Dev);
    if (FAILED(hr)) return hr;

    auto fnCreateD3DVertexBuf = [&spD3D11Dev](const vector<ColorPoint>& vVertices, ComPtr<ID3D11Buffer>& spVertices)->HRESULT
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

        return spD3D11Dev->CreateBuffer(&descVertex, &xinit, &spVertices);
    };

    hr = fnCreateD3DVertexBuf(m_vSideVertices, m_spSideVertices);
    if (FAILED(hr)) return hr;

    hr = fnCreateD3DVertexBuf(m_vBottomVertices, m_spBottomVertices);
    if (FAILED(hr)) return hr;

    // 索引缓冲区
    D3D11_BUFFER_DESC descCubeIndex;
    ZeroMemory(&descCubeIndex, sizeof(D3D11_BUFFER_DESC));
    descCubeIndex.ByteWidth = static_cast<UINT>(sizeof(UINT) * m_vBottomIndexes.size());
    descCubeIndex.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA xinit;;
    ZeroMemory(&xinit, sizeof(D3D11_SUBRESOURCE_DATA));
    xinit.pSysMem = m_vBottomIndexes.data();
    xinit.SysMemPitch = 0;
    xinit.SysMemSlicePitch = 0;

    hr = spD3D11Dev->CreateBuffer(&descCubeIndex, &xinit, &m_spBottomIndexes);
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

void Arc4::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spImCtx, const Space& space)
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

    // 四段拼成
    for (int i = 0; i < 4; i++) {
        XMMATRIX mRotate = XMMatrixRotationY(XM_PIDIV2 * i - XM_PIDIV4);
        constBuf.mWorldViewProjection = XMMatrixTranspose(mRotate * space.mWorld * space.mView * space.mProjection);
        constBuf.nTextured = 0;
        spImCtx->UpdateSubresource(m_spConstBuf.Get(), 0, nullptr, &constBuf, 0, 0);
        spImCtx->PSSetShaderResources(0, 1, m_vTex2D[i].spD3D11SRV.GetAddressOf());

        // bottom
        // spImCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        spImCtx->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
        spImCtx->IASetVertexBuffers(0, 1, m_spBottomVertices.GetAddressOf(), &stride, &offset);
        spImCtx->IASetIndexBuffer(m_spBottomIndexes.Get(), DXGI_FORMAT_R32_UINT, 0);
        spImCtx->DrawIndexed(static_cast<UINT>(m_vBottomIndexes.size()), 0, 0);

        // side
        constBuf.nTextured = 1;
        spImCtx->UpdateSubresource(m_spConstBuf.Get(), 0, nullptr, &constBuf, 0, 0);
        spImCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        spImCtx->IASetVertexBuffers(0, 1, m_spSideVertices.GetAddressOf(), &stride, &offset);
        spImCtx->Draw(static_cast<UINT>(m_vSideVertices.size()), 0);
    }
}

void Arc4::init()
{
    XMFLOAT4 xmf4Color = XMFLOAT4(0.8f, 0.6f, 0.0f, 1.0f);
    XMFLOAT4 xmf4Up(0.0f, 1.0f, 0.0f, 1.0f);
    // center point of bottom
    m_vBottomVertices.push_back({ XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), xmf4Color, xmf4Up, XMFLOAT2(0.0f, 1.0f) });

    float fy0 = 0.0f, fy1 = m_fHeight;
    for (int i = 0; i <= m_nStepsArc; i++) {
        float fTheta = XM_PIDIV2 * i / m_nStepsArc; // 方位角
        float fx1 = XMScalarSin(fTheta);
        float fz1 = XMScalarCos(fTheta);;
        float fx = m_fRadius * fx1;
        float fz = m_fRadius * fz1;
        float fU = fTheta / XM_PIDIV2;

        m_vBottomVertices.push_back({ XMFLOAT4(fx, 0.0f, fz, 1.0f), xmf4Color, xmf4Up, XMFLOAT2(0.0f, 0.5f) });

        XMFLOAT4 xmf4ToCenter(-fx1, 0.0f, -fz1, 1.0f); // 指向柱心
        m_vSideVertices.push_back({ XMFLOAT4(fx, fy0, fz, 1.0f), xmf4Color, xmf4ToCenter, XMFLOAT2(fU, (m_fHeight - fy0) / m_fHeight - 0.5f) });
        m_vSideVertices.push_back({ XMFLOAT4(fx, fy1, fz, 1.0f), xmf4Color, xmf4ToCenter, XMFLOAT2(fU, (m_fHeight - fy1) / m_fHeight) });
        if (i == 0) continue;
        // triangle list for bottom
        m_vBottomIndexes.push_back(0);
        m_vBottomIndexes.push_back(i);
        m_vBottomIndexes.push_back(i + 1);
        m_vBottomIndexes.push_back(0);
    }
}
