#include "stdafx.h"
#include "CamCali.h"

CamCali::CamCali()
{
}

void CamCali::OpenCameraImage(const std::string& pathJPG)
{
	m_image = cv::imread(pathJPG);

	UpdateCoefficients(1500.0f);
}

Gdiplus::Bitmap* CamCali::GetBMP()
{
	return m_uprtBMP.get();
}


void CamCali::UpdateCoefficients(float f)
{
	cv::Size size = m_image.size();
	cv::Mat k = (cv::Mat_<float>(3, 3) << f, 0.0, size.width / 2.0f, 0.0, f, size.height / 2.0f, 0.0, 0.0, 1.0);

	cv::Mat d0 = (cv::Mat_<double>(1, 4) << 0, 0, 0, 0);
	// cv::Mat dx = (cv::Mat_<float>(1, 4) << -3.6f, -0.0f, 1.0f, 0.5f);

	cv::fisheye::undistortImage(m_image, m_imageOut, k, d0, k, size);

	m_uprtBMP.reset();
	m_uprtBMP = std::unique_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(size.width, size.height, static_cast<int>(m_imageOut.step1()), PixelFormat24bppRGB, m_imageOut.data));
}