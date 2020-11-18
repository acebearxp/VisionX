#pragma once

// 纹理贴图
class Karmeliet
{
public:
	Karmeliet();

	// 载入图像
	bool LoadImage(const std::string& strPath);
	// 创建纹理
	HRESULT CreateTexture2D(Microsoft::WRL::ComPtr<ID3D11Device>& spD3D11Dev, Microsoft::WRL::ComPtr<ID3D11Texture2D>& spTexture2D);
private:
	cv::Mat m_image;
};

