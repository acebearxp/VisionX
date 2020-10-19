// ��ͷ����
#pragma once

class CamCali
{
public:
	CamCali();

	// �򿪴��л����ͼ���ļ�
	void OpenCameraImage(const std::string& pathJPG);

	// ��ȡ�������ͼ��
	Gdiplus::Bitmap* GetBMP();

	// �޸ľ�ͷ����
	void UpdateCoefficients(float f);
private:
	// ��ͷ����K
	cv::Mat m_k;
	// ��ͷ����D
	cv::Mat m_d;

	// ��������ͼ��
	cv::Mat m_image;
	// �������ͼ��
	cv::Mat m_imageOut;
	// ������ʾ����Ļ��
	std::unique_ptr<Gdiplus::Bitmap> m_uprtBMP;
};


