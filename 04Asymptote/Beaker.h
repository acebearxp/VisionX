#pragma once
#include "Optica.h"
#include "Spatial.h"

// ������һ���ض�����λ����һ���ض���ͷ�����ƽ��ͼ��
class Beaker
{
public:
	Beaker();

	const std::string& GetPath() const { return m_strPath; }
	const cv::Mat& GetImage()const { return m_image; };
	
public:
	// ��colorָ������ɫ��ʼ��ͼ��
	void Load(int width, int height, const cv::Vec3b& color);
	// ʹ��ָ�����ļ���Ϊͼ��
	void Load(const std::string& path);

	// ����ָ��λ�õ�������ɫֵ
	std::unique_ptr<cv::Vec3b> LookupPixel(float fTheta, int x, int y) const;

	// �趨��λ��(����,N=0 E=Pi/2)
	void SetAzimuth(float fAzimuth) { m_spatial.SetAzimuth(fAzimuth); }
	void SetYaw(float fYaw) { m_spatial.SetYaw(fYaw); }
	// �趨������(����,�����¸�)
	void SetPitch(float fPitch) { m_spatial.SetPitch(fPitch); }
	float GetPitch() const { return m_spatial.GetPitch(); }
	// �趨��ѧ��ͷ
	void SetOptica(std::unique_ptr<Optica>&& uptrOptica);
	// �趨����
	void SetFocus135(float f135);
	// �趨���۾�ͷ����
	void SetFisheyeK(const std::vector<float>& vk);

public:
	// ����ֱ��
	void CopyImage(const Beaker& src);
	// ��ѧ����
	void OpticalTransfer(const Beaker& src, const RECT& rc);
	// �ѱ�����ֵת��ΪԴ����ֵ
	std::tuple<int, int> SpatialTransfer(int x, int y, const Beaker& src);
private:
	// �ռ��ϵ
	Spatial m_spatial;
	// ��ѧ��ͷ
	std::unique_ptr<Optica> m_uptrOptica;

	std::string m_strPath; // ͼ��·��
	cv::Mat m_image; // ͼ��

	// ����У��
	void trapezoidAdjust(int&x, int y, int width, int height, float fPitch);
};

