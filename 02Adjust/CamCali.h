// ��ͷ����
#pragma once

class CamCali
{
public:
	CamCali();

	// �򿪴��л����ͼ���ļ�
	void OpenCameraImage(const std::string& pathJPG);

	// ��ȡ�������ͼ��
	std::shared_ptr<Gdiplus::Bitmap>& GetBMP();

	// �޸ľ�ͷ����
	void UpdateCoefficients(cv::Mat& k, cv::Mat& d);
private:
	// ��ͷ����K
	cv::Mat m_k;
	// ��ͷ����D
	cv::Mat m_d;

	// ��������ͼ��
	cv::Mat m_image;
	// ������ʾ����Ļ��
	std::shared_ptr<Gdiplus::Bitmap> m_sptrBMP;
};


