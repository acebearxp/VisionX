#include "stdafx.h"
#include "OpticaFisheyeTanHalf.h"

using namespace std;

OpticaFisheyeTanHalf::OpticaFisheyeTanHalf()
{
	// Ĭ��ȫ��180�ӽ�, �������۾�ͷ���Ա�180�ȸ�����ӽ�
	constexpr float fAngleFull = static_cast<float>(M_PI);
	OpticaFisheyeTanHalf(c_fWidthFullFrame / (2.0f * tanf(fAngleFull / 4.0f)));
}

OpticaFisheyeTanHalf::OpticaFisheyeTanHalf(float f135)
	: Optica(f135)
{
}

void OpticaFisheyeTanHalf::MakeTheta(int width, int height)
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

			// ��ƽ���۾�ͷ r=2f tan����/2��
			m_uptrTheta->at<float>(y, x) = 2.0f * atanf(sqrtf(fU * fU + fV * fV) / (2.0f * fFocus));
		}
	}
}

float OpticaFisheyeTanHalf::CalcRFromTheta(float fTheta, int width)
{
	// ����(������Ϊ��λ)
	float fFocus = width * m_f135 / c_fWidthFullFrame;

	// ��ƽ���۾�ͷ r=2f tan����/2�� 
	return 2.0f * fFocus * tanf(fTheta / 2.0f);
}