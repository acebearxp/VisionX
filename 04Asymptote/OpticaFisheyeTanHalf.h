#pragma once
#include "Optica.h"

// ��ƽ���۾�ͷ
class OpticaFisheyeTanHalf : public Optica
{
public:
	OpticaFisheyeTanHalf();
	OpticaFisheyeTanHalf(float f135);

	// ���ݹ�ѧģ��,��theta����R(����)
	float CalcRFromTheta(float fTheta, int width) override;
};

