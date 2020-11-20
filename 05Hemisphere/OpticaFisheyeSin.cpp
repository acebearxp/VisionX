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

float OpticaFisheyeSin::CalcRFromTheta(float fTheta, int width)
{
	// ����(������Ϊ��λ)
	float fFocus = width * m_f135 / c_fWidthFullFrame;

	// �������۾�ͷ r = f * sin(theta)
	return fFocus * sinf(fTheta);
}
