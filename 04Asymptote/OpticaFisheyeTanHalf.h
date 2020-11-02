#pragma once
#include "Optica.h"

class OpticaFisheyeTanHalf : public Optica
{
public:
	OpticaFisheyeTanHalf();
	OpticaFisheyeTanHalf(float f135);

	// ����thetaϵ����
	void MakeTheta(int width, int height) override;
	// ���ݹ�ѧģ��,��theta����R(����)
	float CalcRFromTheta(float fTheta, int width) override;
};

