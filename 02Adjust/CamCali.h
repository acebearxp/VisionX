// 镜头修正
#pragma once

class CamCali
{
public:
	CamCali();

	// 打开带有畸变的图像文件
	void OpenCameraImage(const std::string& pathJPG);

	// 获取修正后的图像
	std::shared_ptr<Gdiplus::Bitmap>& GetBMP();

	// 修改镜头参数
	void UpdateCoefficients(cv::Mat& k, cv::Mat& d);
private:
	// 镜头参数K
	cv::Mat m_k;
	// 镜头参数D
	cv::Mat m_d;

	// 待修正的图像
	cv::Mat m_image;
	// 用于显示到屏幕上
	std::shared_ptr<Gdiplus::Bitmap> m_sptrBMP;
};


