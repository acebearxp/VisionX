#include "stdafx.h"
#include "Sphere4.h"
#include "Karmeliet.h"

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

    hr = fnCreateD3DBuf(m_vSideVertices, m_vSideIndexes, m_spSideVertices, m_spSideIndexes);
    if (FAILED(hr)) return hr;

    // texture
    vector<string> vPaths = {
        R"(D:/VisionX/05Hemisphere/carset2/set1/1front.jpg)", // Pi*0/2
        R"(D:/VisionX/05Hemisphere/carset2/set1/2right.jpg)", // Pi*1/2
        R"(D:/VisionX/05Hemisphere/carset2/set1/3back.jpg)",  // Pi*2/2
        R"(D:/VisionX/05Hemisphere/carset2/set1/4left.jpg)"   // Pi*3/2
    };
    for (const string& strPath : vPaths) {
        Karmeliet kaImage;
        kaImage.LoadTexture(strPath);
        kaImage.applyAlpha(0.4f, 0.85f);

        Texture2DResource tex2DRes;
        hr = loadTexture2D(spD3D11Dev, kaImage.GetData(), tex2DRes.spD3D11Tex2D, tex2DRes.spD3D11SRV);
        if (FAILED(hr)) return hr;

        m_vTex2D.push_back(tex2DRes);
    }
    // 位置
    m_vCamPos = {
        {  836.0f,    0.0f, 1082.0f }, // front
        { 2597.0f,  972.0f, 1539.0f }, // right
        { 5540.0f,    0.0f, 1534.0f }, // back
        { 2556.0f, -997.0f, 1534.0f }  // left
    };
    XMFLOAT3 origin = {
        (m_vCamPos[0].x + m_vCamPos[2].x) / 2.0f,
        (m_vCamPos[1].y + m_vCamPos[3].y) / 2.0f,
        m_vCamPos[0].z
    };
    for (XMFLOAT3& pos : m_vCamPos) {
        // 以车辆正中心为坐标原点
        XMFLOAT3 pos3;
        XMVECTOR pos2 = XMVectorSubtract(XMLoadFloat3(&pos), XMLoadFloat3(&origin));
        pos2 = XMVectorScale(pos2, 0.01f);
        XMStoreFloat3(&pos3, pos2);
        // 坐标系定义不同,转换一下
        pos.x =  pos3.y;
        pos.y =  pos3.z;
        pos.z = -pos3.x;
    }

    // 下倾角
    // m_vPitch = { 17.0f, 42.0f, 31.0f, 44.0f };
    m_vPitch = { 17.0f, 20.0f, 17.0f, 20.0f };
    for (float& fPitch : m_vPitch) {
        fPitch = XM_PI * fPitch / 180.0f; // 弧度
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
    for (int i = 0; i < 4; i++) {
        int x = vSequence[i];
        XMMATRIX mRotate = XMMatrixRotationX(m_vPitch[x]) * XMMatrixRotationY(XM_PIDIV2 * x);
        float fScale = (x % 2 == 0) ? 1.02f : 1.0f; // 0,2前后略远 1,3右左略近(for blending) 
        XMMATRIX mScale = XMMatrixScaling(fScale, 1.0f, fScale);
        XMMATRIX mMove = XMMatrixTranslation(m_vCamPos[x].x, m_vCamPos[x].y, m_vCamPos[x].z);
        constBuf.mWorldViewProjection = XMMatrixTranspose(mRotate * mScale * space.mWorld * space.mView * space.mProjection);

        // side
        constBuf.nTextured = 1;
        spImCtx->UpdateSubresource(m_spConstBuf.Get(), 0, nullptr, &constBuf, 0, 0);
        spImCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        spImCtx->IASetVertexBuffers(0, 1, m_spSideVertices.GetAddressOf(), &stride, &offset);
        spImCtx->IASetIndexBuffer(m_spSideIndexes.Get(), DXGI_FORMAT_R32_UINT, 0);
        spImCtx->PSSetShaderResources(0, 1, m_vTex2D[x].spD3D11SRV.GetAddressOf());
        spImCtx->DrawIndexed(static_cast<UINT>(m_vSideIndexes.size()), 0, 0);
    }
}

void Sphere4::init()
{
	XMFLOAT4 xmf4Color = XMFLOAT4(0.8f, 0.6f, 0.0f, 1.0f);
	XMFLOAT4 xmf4Up(0.0f, 1.0f, 0.0f, 1.0f);

	float fStep = 2.0f * XM_PI / m_nStepsArc;
	for (int i = 0; i <= m_nStepsArc / 4; i++) {
		if (i == 0) {
			// 中心点
			m_vSideVertices.push_back({ XMFLOAT4(0.0f, 0.0f, m_fRadius, 1.0f), xmf4Color, xmf4Up, XMFLOAT2(0.5f, 0.5f) });
		}
		else {
			float fPitch = fStep * i;
            float fs = XMScalarSin(fPitch);
			float fz = m_fRadius * XMScalarCos(fPitch);
			float fr = m_fRadius * fs;
			for (int j = 0; j < m_nStepsArc; j++) {
				float fTheta = fStep * j;
                float fsin = XMScalarSin(fTheta);
                float fcos = XMScalarCos(fTheta);

				float fx = fr * fsin;
				float fy = fr * fcos;
                
                /* 考虑鱼眼(sin模型)变形;而普通标准镜头是tan模型 fr2 = fz * tanf(fPitch)
                   float fr2 = fz * XMScalarSin(fPitch);
                   float fU = 0.5f + 0.5f * fr2 * fsin / fz;
                   float fV = 0.5f - 0.5f * fr2 * fcos * 1.5f / fz;
                   化简后如下: */
                float fU = 0.5f + 0.5f * fsin * fs;
                float fV = 0.5f - 0.5f * fcos * fs * 1.5f; // 垂直方向上视张角120度,水平180度,所以垂直方向上有个1.5f的系数

				m_vSideVertices.push_back({ XMFLOAT4(fx, fy, fz, 1.0f), xmf4Color, xmf4Up, XMFLOAT2(fU, fV) });
				if (i == 1) {
					// 最内圈
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
					// 外圈
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
