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

float Optica::CalcTheta(int x, int y, int width, int height)
{
	// 中心点(以像素为单位)
	const float fX0 = width / 2.0f;
	const float fY0 = height / 2.0f;

	// 焦距(以像素为单位)
	float fFocus = width * m_f135 / c_fWidthFullFrame;
	
	float fU = x - fX0;
	float fV = y - fY0;

	// 标准镜头 r = f * tan(theta)
	float fTheta = atan2f(sqrtf(fU * fU + fV * fV), fFocus);

	return fTheta;
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

tuple<int,int> Optica::SpatialTransfer(int x, int y, int width, int height, const SpatialDiff& delta)
{
	// 中心点(以像素为单位)
	const float fX0 = width / 2.0f;
	const float fY0 = height / 2.0f;

	// 焦距(以像素为单位)
	float fFocus = x * m_f135 / c_fWidthFullFrame;

	float fU = x - fX0;
	float fV = y - fY0;

	float fRaw = atan2f(fU, fFocus);

	float fU2 = tanf(fRaw + delta.fDeltaAzimuth) * fFocus;
	float fV2 = fV;

	int x2 = static_cast<int>(fU2 + fX0);
	int y2 = static_cast<int>(fV2 + fY0);
	
	return tuple<int, int>(x2, y2);
}
