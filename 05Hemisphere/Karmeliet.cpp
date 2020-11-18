#include "stdafx.h"
#include "Karmeliet.h"

using namespace std;

Karmeliet::Karmeliet()
{
}

bool Karmeliet::LoadImage(const string& strPath)
{
	// BGR
	cv::Mat image = cv::imread(strPath, cv::ImreadModes::IMREAD_COLOR);
	cv::cvtColor(image, m_image, cv::ColorConversionCodes::COLOR_BGR2RGBA);
	return true;
}

HRESULT Karmeliet::CreateTexture2D(Microsoft::WRL::ComPtr<ID3D11Device>& spD3D11Dev, Microsoft::WRL::ComPtr<ID3D11Texture2D>& spTexture2D)
{
	D3D11_TEXTURE2D_DESC desc2D = {
		static_cast<UINT>(m_image.cols),
		static_cast<UINT>(m_image.rows),
		1,
		1,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		{ 1, 0 },
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE,
		0,
		0
	};

	D3D11_SUBRESOURCE_DATA xinit;
	xinit.pSysMem = m_image.data;
	xinit.SysMemPitch = 4 * m_image.cols;
	xinit.SysMemSlicePitch = 0;

	HRESULT hr = spD3D11Dev->CreateTexture2D(&desc2D, &xinit, spTexture2D.GetAddressOf());
	return hr;
}
