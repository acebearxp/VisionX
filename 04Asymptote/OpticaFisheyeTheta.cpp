#include "stdafx.h"
#include "OpticaFisheyeTheta.h"

using namespace std;

OpticaFisheyeTheta::OpticaFisheyeTheta()
{
	// Ĭ��ȫ��180�ӽ�, �������۾�ͷ���Ա�180�ȸ�����ӽ�
	constexpr float fAngleFull = static_cast<float>(M_PI);
	OpticaFisheyeTheta(c_fWidthFullFrame / fAngleFull);
}

OpticaFisheyeTheta::OpticaFisheyeTheta(float f135)
	: Optica(f135)
{
}

float OpticaFisheyeTheta::CalcRFromTheta(float fTheta, int width)
{
	// ����(������Ϊ��λ)
	float fFocus = width * m_f135 / c_fWidthFullFrame;

	// �Ƚ����۾�ͷ r = f*��(k0+k1*��^2+k2*��^4+k3*��^6+k4*��^8)
	float fSum = 0.0f;
	for (int i = 0; i < m_vk.size(); i++) {
		fSum += m_vk[i] * static_cast<float>(pow(fTheta, 2 * i));
	}
	return fFocus * fTheta * fSum;
}

void OpticaFisheyeTheta::SetFisheyeK(const std::vector<float>& vk)
{
	m_vk = vk;
}
