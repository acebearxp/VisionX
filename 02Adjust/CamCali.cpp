#include "stdafx.h"
#include "CamCali.h"

CamCali::CamCali()
{
}

void CamCali::OpenCameraImage(const std::string& pathJPG)
{
	m_image = cv::imread(pathJPG);
}

std::shared_ptr<Gdiplus::Bitmap>& CamCali::GetBMP()
{
	if (!m_image.empty()) {
		m_sptrBMP.reset();
		cv::Size size = m_image.size();
		m_sptrBMP = std::shared_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(size.width, size.height, static_cast<int>(m_image.step1()), PixelFormat24bppRGB, m_image.data));
	}
	return m_sptrBMP;
}


void CamCali::UpdateCoefficients(cv::Mat& k, cv::Mat& d)
{
	// cv::Mat outX;
	// cv::fisheye::undistortImage(m_image, outX, k, d, k, m_image.size());
}