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

float OpticaFisheyeTanHalf::CalcRFromTheta(float fTheta, int width)
{
	// ����(������Ϊ��λ)
	float fFocus = width * m_f135 / c_fWidthFullFrame;

	// ��ƽ���۾�ͷ r=2f tan����/2�� 
	return 2.0f * fFocus * tanf(fTheta / 2.0f);
}