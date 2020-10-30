#include "stdafx.h"
#include "Optica.h"

using namespace std;

Optica::Optica()
{
}

Optica::Optica(float f135)
	:m_f135(f135)
{
}

float Optica::FindTheta(int x, int y)
{
	return m_uptrTheta->at<float>(y, x);
}

void Optica::MakeTheta(int width, int height)
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

			// 标准镜头 r = f * tan(theta)
			m_uptrTheta->at<float>(y, x) = atan2f(sqrtf(fU * fU + fV * fV), fFocus);
		}
	}
}

float Optica::CalcRFromTheta(float fTheta, int width)
{
	// 焦距(以像素为单位)
	float fFocus = width * m_f135 / c_fWidthFullFrame;

	// 标准镜头 r = f * tan(theta)
	return fFocus * tanf(fTheta);
}
