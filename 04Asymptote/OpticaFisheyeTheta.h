#pragma once
#include "Optica.h"

/* �������ĵȽ����۾�ͷ
*  r = f*��(k0+k1*��^2+k2*��^4+k3*��^6+k4*��^8)
*  ��k0=1ʱ,��opencvʹ�õ�����ģ��һ��
*/
class OpticaFisheyeTheta : public Optica
{
public:
	OpticaFisheyeTheta();
	OpticaFisheyeTheta(float f135);

	// �趨ϵ��ֵ
	void SetK(const std::vector<float>& vk) { m_vk = vk; }

	// ���ݹ�ѧģ��,��theta����R(����)
	float CalcRFromTheta(float fTheta, int width) override;
	// �趨����
	void SetFisheyeK(const std::vector<float>& vk) override;
private:
	std::vector<float> m_vk = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f };
};

