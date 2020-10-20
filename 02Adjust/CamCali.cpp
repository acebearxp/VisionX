#include "stdafx.h"
#include "CamCali.h"

CamCali::CamCali()
{
}

void CamCali::OpenCameraImage(const std::string& pathJPG)
{
	m_image = cv::imread(pathJPG).getUMat(cv::AccessFlag::ACCESS_READ);
}

Gdiplus::Bitmap* CamCali::GetBMP()
{
	return m_uprtBMP.get();
}


void CamCali::UpdateCoefficients(float f, const std::vector<float>& k)
{
	cv::Size size = m_image.size();
	cv::Mat A2 = (cv::Mat_<float>(3, 3) << f, 0.0, size.width / 2.0f, 0.0, f, size.height / 2.0f, 0.0, 0.0, 1.0);
	cv::UMat A = A2.getUMat(cv::AccessFlag::ACCESS_READ);
    cv::Mat d2 = (cv::Mat_<double>(1, 4) << k[0], k[1], k[2], k[3]);
	cv::UMat d = d2.getUMat(cv::AccessFlag::ACCESS_READ);
	
	cv::fisheye::undistortImage(m_image, m_imageOutG, A, d, A, size);
	// m_imageOut = m_imageOutG.getMat(cv::AccessFlag::ACCESS_READ);
	m_imageOutG.copyTo(m_imageOut);

	m_uprtBMP.reset();
	m_uprtBMP = std::unique_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(size.width, size.height, static_cast<int>(m_imageOut.step1()), PixelFormat24bppRGB, m_imageOut.data));
}