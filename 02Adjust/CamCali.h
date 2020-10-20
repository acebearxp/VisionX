// 镜头修正
#pragma once

class CamCali
{
public:
	CamCali();

	// 打开带有畸变的图像文件
	void OpenCameraImage(const std::string& pathJPG);

	// 获取修正后的图像
	Gdiplus::Bitmap* GetBMP();

	// 修改镜头参数
	void UpdateCoefficients(float f, const std::vector<float>& k);
private:
	// 镜头参数K
	cv::Mat m_k;
	// 镜头参数D
	cv::Mat m_d;

	// 待修正的图像
	cv::UMat m_image;
	// 修正后的图像
	cv::UMat m_imageOutG;
	cv::Mat m_imageOut;
	// 用于显示到屏幕上
	std::unique_ptr<Gdiplus::Bitmap> m_uprtBMP;
};


