#include "stdafx.h"
#include "OpticaFisheyeArea.h"

using namespace std;

OpticaFisheyeArea::OpticaFisheyeArea()
{
	// Ĭ��ȫ��180�ӽ�, �������۾�ͷ���Ա�180�ȸ�����ӽ�
	constexpr float fAngleFull = static_cast<float>(M_PI);
	OpticaFisheyeArea(c_fWidthFullFrame / (2.0f * sinf(fAngleFull / 4.0f)));
}

OpticaFisheyeArea::OpticaFisheyeArea(float f135)
	: Optica(f135)
{
}

float OpticaFisheyeArea::CalcRFromTheta(float fTheta, int width)
{
	// ����(������Ϊ��λ)
	float fFocus = width * m_f135 / c_fWidthFullFrame;

	// ��������۾�ͷ r=2f sin����/2�� 
	return 2.0f * fFocus * sinf(fTheta / 2.0f);
}
