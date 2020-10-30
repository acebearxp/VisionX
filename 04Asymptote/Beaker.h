#pragma once
#include "Optica.h"

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

	// �趨��ѧ��ͷ
	void SetOptica(std::unique_ptr<Optica>&& uptrOptica);

public:
	// ����ֱ��
	void CopyImage(const Beaker& src);
	// ��ѧ����
	void OpticalTransfer(const Beaker& src);
private:
	// TODO: �������
	// ��ѧ��ͷ
	std::unique_ptr<Optica> m_uptrOptica;

	std::string m_strPath; // ͼ��·��
	cv::Mat m_image; // ͼ��
};

