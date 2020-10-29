#include "stdafx.h"
#include "CVBitmap.h"

using namespace std;

CVBitmap::CVBitmap(const cv::Mat& source)
	:c_mat(source.clone())
{
	cv::Size size = c_mat.size();
	m_uptrBMP = unique_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(size.width, size.height, static_cast<int>(c_mat.step1()), PixelFormat24bppRGB, c_mat.data));
}
