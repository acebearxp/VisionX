#include "stdafx.h"
#include "OpticaFisheyeArea.h"

using namespace std;

OpticaFisheyeArea::OpticaFisheyeArea()
{
	// Ĭ��ȫ��180�ӽ�, �������۾�ͷ���Ա�180�ȸ�����ӽ�
	const float fAngleFull = 3.14159265358f;
	OpticaFisheyeArea(c_fWidthFullFrame / (2.0f * sinf(fAngleFull / 4.0f)));
}

OpticaFisheyeArea::OpticaFisheyeArea(float f135)
	: Optica(f135)
{
}

void OpticaFisheyeArea::MakeTheta(int width, int height)
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

			// ��������۾�ͷ r=2f sin����/2��
			m_uptrTheta->at<float>(y, x) = 2.0f * asinf(sqrtf(fU * fU + fV * fV) / (2.0f * fFocus));
		}
	}
}

float OpticaFisheyeArea::CalcRFromTheta(float fTheta, int width)
{
	// ����(������Ϊ��λ)
	float fFocus = width * m_f135 / c_fWidthFullFrame;

	// ��������۾�ͷ r=2f sin����/2�� 
	return 2.0f * fFocus * sinf(fTheta / 2.0f);
}
