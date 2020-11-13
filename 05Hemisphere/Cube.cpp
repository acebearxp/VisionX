#include "stdafx.h"
#include "Cube.h"

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

CCube::CCube()
{
    // 一个立方体有8个顶点 POSITION, COLOR
    m_vVertexes = {
        { XMFLOAT4(-1.0f,  1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT4( 1.0f,  1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT4( 1.0f,  1.0f,  1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT4(-1.0f,  1.0f,  1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT4(-1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT4( 1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT4( 1.0f, -1.0f,  1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT4(-1.0f, -1.0f,  1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) }
    };

    // 转换为一系列的三角形,立方体有6个面,每个面拆成2个三角形,三角形3个点,共计3*2*6=36个点
    m_vIndexes = {
        3, 1, 0,
        2, 1, 3,
        0, 5, 4,
        1, 5, 0,
        3, 4, 7,
        0, 4, 3,
        1, 6, 5,
        2, 6, 1,
        2, 7, 6,
        3, 7, 2,
        6, 4, 5,
        7, 4, 6
    };
}

HRESULT CCube::CreateD3DBuf(ComPtr<ID3D11Device>& spD3D11)
{
    // 顶点缓冲区
    D3D11_BUFFER_DESC descVertex;
    ZeroMemory(&descVertex, sizeof(D3D11_BUFFER_DESC));
    descVertex.ByteWidth = sizeof(ColorPoint) * static_cast<UINT>(m_vVertexes.size());
    descVertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA xinit;;
    ZeroMemory(&xinit, sizeof(D3D11_SUBRESOURCE_DATA));
    xinit.pSysMem = m_vVertexes.data();
    xinit.SysMemPitch = 0;
    xinit.SysMemSlicePitch = 0;

    HRESULT hr = spD3D11->CreateBuffer(&descVertex, &xinit, &m_spVertex);
    if (FAILED(hr)) return hr;

    // 索引缓冲区
    D3D11_BUFFER_DESC descCubeIndex;
    ZeroMemory(&descCubeIndex, sizeof(D3D11_BUFFER_DESC));
    descCubeIndex.ByteWidth = sizeof(UINT) * 36;
    descCubeIndex.BindFlags = D3D11_BIND_INDEX_BUFFER;

    xinit.pSysMem = m_vIndexes.data();

    hr = spD3D11->CreateBuffer(&descCubeIndex, &xinit, &m_spCubeIndex);
    if (FAILED(hr)) return hr;

    return S_OK;
}
