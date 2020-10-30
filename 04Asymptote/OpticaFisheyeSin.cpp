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
	// ���ĵ�(������Ϊ��λ)
	const float fX0 = width / 2.0f;
	const float fY0 = height / 2.0f;

	// ����(������Ϊ��λ)
	float fFocus = width * m_f135 / c_fWidthFullFrame;

	m_uptrTheta = make_unique<cv::Mat>(cv::Mat(height, width, CV_32F));
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			float fU = x - fX0;
			float fV = y - fY0;

			// �������۾�ͷ r = f * sin(theta)
			m_uptrTheta->at<float>(y, x) = asinf(sqrtf(fU * fU + fV * fV) / fFocus);
		}
	}
}

float OpticaFisheyeSin::CalcRFromTheta(float fTheta, int width)
{
	// ����(������Ϊ��λ)
	float fFocus = width * m_f135 / c_fWidthFullFrame;

	// �������۾�ͷ r = f * sin(theta)
	return fFocus * sinf(fTheta);
}
