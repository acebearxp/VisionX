#pragma once

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

public:
	// ����ֱ��
	void CopyImage(const Beaker& src);
	// ��ѧ����
	void OpticalTransfer(const Beaker& src);
private:
	// TODO: ��ͷ����
	// TODO: �������
	std::string m_strPath; // ͼ��·��
	cv::Mat m_image; // ͼ��
};

