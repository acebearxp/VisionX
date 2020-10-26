#pragma once

using namespace std;

// 容纳单张图片的处理数据
class TeaPot
{
public:
	TeaPot(const string& path);
	virtual ~TeaPot();

	const unique_ptr<Gdiplus::Bitmap>& GetBMP() const { return m_uptrBmp; }

	// 加载图像
	void LoadImage();
	// 鱼眼镜头修正
	void Calibrate(float focus, const vector<float>& d);

private:
	// 图像文件路径
	const string m_strPath;
	// 原始图像
	cv::Mat m_image;
	// 输出图像
	cv::Mat m_imageOut;
	// 输出图像BMP
	unique_ptr<Gdiplus::Bitmap> m_uptrBmp;

	// 转换成BITMAP
	static unique_ptr<Gdiplus::Bitmap> FromOpenCVImage(const cv::Mat& image);
};

