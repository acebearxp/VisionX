#include "stdafx.h"
#include "OpticaFisheyeSin.h"

using namespace std;

OpticaFisheyeSin::OpticaFisheyeSin()
	: Optica(38.0f)
{
}

OpticaFisheyeSin::OpticaFisheyeSin(float f135)
	: Optica(f135)
{
}

void OpticaFisheyeSin::MakeTheta(int width, int height)
{
	// 中心点(以像素为单位)
	const float fX0 = width / 2.0f;
	const float fY0 = height / 2.0f;

	// 焦距(以像素为单位)
	float fFocus = width * m_f135 / c_fWidthFullFrame;

	m_uptrTheta = make_unique<cv::Mat>(cv::Mat(height, width, CV_32F));
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			float fU = x - fX0;
			float fV = y - fY0;

			// 正弦鱼眼镜头 r = f * sin(theta)
			m_uptrTheta->at<float>(y, x) = asinf(sqrtf(fU * fU + fV * fV) / fFocus);
		}
	}
}

float OpticaFisheyeSin::CalcRFromTheta(float fTheta, int width)
{
	// 焦距(以像素为单位)
	float fFocus = width * m_f135 / c_fWidthFullFrame;

	// 正弦鱼眼镜头 r = f * sin(theta)
	return fFocus * sinf(fTheta);
}
