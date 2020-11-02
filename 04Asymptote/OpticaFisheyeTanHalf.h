#pragma once
#include "Optica.h"

class OpticaFisheyeTanHalf : public Optica
{
public:
	OpticaFisheyeTanHalf();
	OpticaFisheyeTanHalf(float f135);

	// 生成theta系数表
	void MakeTheta(int width, int height) override;
	// 依据光学模型,从theta计算R(像素)
	float CalcRFromTheta(float fTheta, int width) override;
};

