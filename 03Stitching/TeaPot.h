#pragma once

using namespace std;

// ���ɵ���ͼƬ�Ĵ�������
class TeaPot
{
public:
	TeaPot(const string& path);
	virtual ~TeaPot();

	// ����ͼ��
	void LoadImage();

private:
	// ͼ���ļ�·��
	const string m_strPath;
	// ͼ��
	cv::Mat m_image;
};

