#pragma once

// ��׼��ͷ��ѧ��Ԫ
class Optica
{
public:
	Optica();
	Optica(float f135);

	// ����thetaϵ����
	void MakeTheta(int width, int height);
private:
	// 135ȫ������(����)
	const float c_fWidthFullFrame = 36.0f;
	// 135��Ч����(����)
	float m_f135 = 50.0f;
	// ÿһ���Ӧ��thetaֵ,����������ߺ͹���н�(����)
	std::unique_ptr<cv::Mat> m_uptrTheta;
};
