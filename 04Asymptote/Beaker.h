#pragma once

// 包含从一个特定坐标位置用一个特定镜头拍摄的平面图像
class Beaker
{
public:
	Beaker(int width, int height);
	Beaker(const std::string & strPath);

	const std::string& GetPath() const { return c_strPath; }
	const cv::Mat& GetImage()const { return m_image; };

	// 像素直拷
	void Copy(const Beaker& src);
	// 光学传送
	void OpticalTransfer(const Beaker& src);
private:
	const std::string c_strPath; // 图像路径
	cv::Mat m_image; // 原始图像
};

