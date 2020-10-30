#pragma once

#include "Spatial.h"

// ��׼��ͷ��ѧ��Ԫ
class Optica
{
public:
	Optica();
	Optica(float f135);

	// ����thetaϵ����
	float FindTheta(int x, int y);
	// ����theta
	float CalcTheta(int x, int y, int width, int height);
	
	// ����thetaϵ����
	virtual void MakeTheta(int width, int height);
	// ���ݹ�ѧģ��,��theta����R(����)
	virtual float CalcRFromTheta(float fTheta, int width);

	// ����ռ�任�������ֵ
	std::tuple<int,int> SpatialTransfer(int x, int y, const SpatialDiff& delta);
protected:
	// 135ȫ�����(����)
	const float c_fWidthFullFrame = 36.0f;
	// 135��Ч����(����)
	float m_f135 = 50.0f;
	// ÿһ���Ӧ��thetaֵ,����������ߺ͹���н�(����)
	std::unique_ptr<cv::Mat> m_uptrTheta;
};

