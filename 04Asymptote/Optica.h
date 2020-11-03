#pragma once

#include "Spatial.h"

// ��׼��ͷ��ѧ��Ԫ
class Optica
{
public:
	Optica();
	Optica(float f135);

	// ����theta
	float CalcTheta(int x, int y, int width, int height);
	// ���ݹ�ѧģ��,��theta����R(����)
	virtual float CalcRFromTheta(float fTheta, int width);

	// ����ռ�任�������ֵ
	std::tuple<int,int> SpatialTransfer(int x, int y, int width, int height, const SpatialDiff& delta);

	// �趨����
	void SetFocus135(float f135) { m_f135 = f135; }
	// �趨���۾�ͷ����
	virtual void SetFisheyeK(const std::vector<float>& vk) {}
protected:
	// 135ȫ�����(����)
	const float c_fWidthFullFrame = 36.0f;
	// 135��Ч����(����)
	float m_f135 = 50.0f;
};

