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
	m_uptrBMP = FromOpenCVImage(m_image);
}

unique_ptr<Gdiplus::Bitmap> TeaPot::FromOpenCVImage(const cv::Mat& image)
{
	cv::Size size = image.size();
	return unique_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(size.width, size.height, static_cast<int>(image.step1()), PixelFormat24bppRGB, image.data));
}
