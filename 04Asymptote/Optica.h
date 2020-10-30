#pragma once

// ��׼��ͷ��ѧ��Ԫ
class Optica
{
public:
	Optica();
	Optica(float f135);

	// ����thetaϵ����
	virtual float FindTheta(int x, int y);
	// ����thetaϵ����
	virtual void MakeTheta(int width, int height);

	// ���ݹ�ѧģ��,��theta����R(����)
	virtual float CalcRFromTheta(float fTheta, int width);
private:
	// 135ȫ�����(����)
	const float c_fWidthFullFrame = 36.0f;
	// 135��Ч����(����)
	float m_f135 = 50.0f;
	// ÿһ���Ӧ��thetaֵ,����������ߺ͹���н�(����)
	std::unique_ptr<cv::Mat> m_uptrTheta;
};

