#pragma once

// ������һ���ض�����λ����һ���ض���ͷ�����ƽ��ͼ��
class Beaker
{
public:
	Beaker(int width, int height);
	Beaker(const std::string & strPath);

	const std::string& GetPath() const { return c_strPath; }
	const cv::Mat& GetImage()const { return m_image; };

	// ����ֱ��
	void Copy(const Beaker& src);
	// ��ѧ����
	void OpticalTransfer(const Beaker& src);
private:
	const std::string c_strPath; // ͼ��·��
	cv::Mat m_image; // ԭʼͼ��
};

