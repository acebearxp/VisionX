#pragma once
#include "Optica.h"

// ��������۾�ͷ r=2f sin����/2�� 
class OpticaFisheyeArea : public Optica
{
public:
	OpticaFisheyeArea();
	OpticaFisheyeArea(float f135);

	// ���ݹ�ѧģ��,��theta����R(����)
	float CalcRFromTheta(float fTheta, int width) override;
};

