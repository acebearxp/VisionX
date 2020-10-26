#pragma once

using namespace std;

// 容纳单张图片的处理数据
class TeaPot
{
public:
	TeaPot(const string& path);
	virtual ~TeaPot();

	const unique_ptr<Gdiplus::Bitmap>& GetBMP() const { return m_uptrBMP; }

	// 加载图像
	void LoadImage();

private:
	// 图像文件路径
	const string m_strPath;
	// 原始图像
	cv::Mat m_image;
	// 原始图像BMP
	unique_ptr<Gdiplus::Bitmap> m_uptrBMP;

	// 转换成BITMAP
	static unique_ptr<Gdiplus::Bitmap> FromOpenCVImage(const cv::Mat& image);
};

