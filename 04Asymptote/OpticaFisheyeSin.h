#pragma once
#include "Optica.h"

// �������۾�ͷ r = f * sin(theta)
class OpticaFisheyeSin : public Optica
{
public:
	OpticaFisheyeSin();
	OpticaFisheyeSin(float f135);

	// ���ݹ�ѧģ��,��theta����R(����)
	float CalcRFromTheta(float fTheta, int width) override;
};

