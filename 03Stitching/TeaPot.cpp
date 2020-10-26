#include "stdafx.h"
#include "TeaPot.h"

TeaPot::TeaPot(const string& path)
	:m_strPath(path)
{
	OutputDebugString(L"===> TeaPot ctor\n");
}

TeaPot::~TeaPot()
{
	OutputDebugString(L"===> TeaPot destroy\n");
}

void TeaPot::LoadImage()
{
	m_image = cv::imread(m_strPath);
	// m_uptrBmp = FromOpenCVImage(m_image);
}

void TeaPot::Calibrate(float focus, const vector<float>& d)
{
	cv::Size size = m_image.size();
	cv::Mat K = (cv::Mat_<float>(3, 3) << focus, 0.0, size.width / 2.0f, 0.0, focus, size.height / 2.0f, 0.0, 0.0, 1.0);
	cv::Mat D = (cv::Mat_<float>(1, 4) << d[0], d[1], d[2], d[3]);

	cv::UMat KGPU = K.getUMat(cv::AccessFlag::ACCESS_READ);
	cv::UMat DGPU = D.getUMat(cv::AccessFlag::ACCESS_READ);

	cv::UMat imageGPU = m_image.getUMat(cv::AccessFlag::ACCESS_READ);

	cv::UMat imageOutGPU;
	cv::fisheye::undistortImage(imageGPU, imageOutGPU, KGPU, DGPU, KGPU, size);
	imageOutGPU.copyTo(m_imageOut);

	m_uptrBmp = FromOpenCVImage(m_imageOut);
}

unique_ptr<Gdiplus::Bitmap> TeaPot::FromOpenCVImage(const cv::Mat& image)
{
	cv::Size size = image.size();
	return unique_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(size.width, size.height, static_cast<int>(image.step1()), PixelFormat24bppRGB, image.data));
}
