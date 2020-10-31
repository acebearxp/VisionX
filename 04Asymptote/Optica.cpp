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
	// ���ĵ�(������Ϊ��λ)
	const float fX0 = width / 2.0f;
	const float fY0 = height / 2.0f;

	// ����(������Ϊ��λ)
	float fFocus = width * m_f135 / c_fWidthFullFrame;
	
	float fU = x - fX0;
	float fV = y - fY0;

	// ��׼��ͷ r = f * tan(theta)
	float fTheta = atan2f(sqrtf(fU * fU + fV * fV), fFocus);

	return fTheta;
}

void Optica::MakeTheta(int width, int height)
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

			// ��׼��ͷ r = f * tan(theta)
			m_uptrTheta->at<float>(y, x) = atan2f(sqrtf(fU * fU + fV * fV), fFocus);
		}
	}
}

float Optica::CalcRFromTheta(float fTheta, int width)
{
	// ����(������Ϊ��λ)
	float fFocus = width * m_f135 / c_fWidthFullFrame;

	// ��׼��ͷ r = f * tan(theta)
	return fFocus * tanf(fTheta);
}

tuple<int,int> Optica::SpatialTransfer(int x, int y, int width, int height, const SpatialDiff& delta)
{
	// (x,y)Ŀ����������� -> ����Դ�Ķ�Ӧ����

	// TODO: ��ǰֻ������ˮƽ�������ת�仯

	// ���ĵ�(������Ϊ��λ)
	const float fX0 = width / 2.0f;
	const float fY0 = height / 2.0f;

	// ����(������Ϊ��λ)
	float fFocus = width * m_f135 / c_fWidthFullFrame;

	// Ŀ��������ĵ������
	float fU = x - fX0;
	float fV = y - fY0;

	// Ŀ��theta��ˮƽ����
	float fThetaH = atan2f(fU, fFocus);

	// Դthetaˮƽ����,������ת����
	float fThetaHRaw = fThetaH + delta.fDeltaAzimuth;

	// ��Ч��(-Pi,+Pi]֮��, ��ת����fhetaHRaw����������ֵ
	while (fThetaHRaw > +M_PI) fThetaHRaw -= static_cast<float>(M_PI + M_PI);
	while (fThetaHRaw <= -M_PI) fThetaHRaw += static_cast<float>(M_PI + M_PI);

	// ��Ҫ���� (-Pi/2,+Pi/2)����Ĳ���
	float fU2, fV2 = fV;
	if (fThetaHRaw > -M_PI_2 && fThetaHRaw < M_PI_2)
		fU2 = fFocus * tanf(fThetaHRaw);
	else
		fU2 = 999999999.0f; // �趨һ���ܴ��ֵ,ʹ�䳬��Դͼ��Χ����

	int x2 = static_cast<int>(fU2 + fX0);
	int y2 = static_cast<int>(fV2 + fY0);
	
	return tuple<int, int>(x2, y2);
}
