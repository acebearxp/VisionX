#pragma once
#include "Optica.h"

// 正弦鱼眼镜头 r = f * sin(theta)
class OpticaFisheyeSin : public Optica
{
public:
	OpticaFisheyeSin();
	OpticaFisheyeSin(float f135);

	// 生成theta系数表
	void MakeTheta(int width, int height) override;
	// 依据光学模型,从theta计算R(像素)
	float CalcRFromTheta(float fTheta, int width) override;
};

