#pragma once

// �ռ���Թ�ϵ
class SpatialDiff
{
public:
	float fDeltaAzimuth = 0.0f;
};

// �ռ��ϵ
class Spatial
{
public:
	Spatial();
	Spatial(float fAzimuth) { m_fAzimuth = fAzimuth; };

	void SetAzimuth(float fAzimuth) { m_fAzimuth = fAzimuth; }

	SpatialDiff operator-(const Spatial& right);

private:
	// ��ʼ��λ��(����, N=0 E=Pi/2 S=Pi W=3Pi/2)
	float m_fAzimuth = 0.0f;
	// ˮƽƫ���(���� ������)
	float m_fYaw = 0.0f;
	// ����ƫ���(���� �����¸�)
	float m_fPitch = 0.0f;
	// TODO: �ռ�λ��(x,y,z)
};

