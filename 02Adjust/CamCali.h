// ��ͷ����
#pragma once

class CamCali
{
public:
	CamCali();
	~CamCali();

	// �򿪴��л����ͼ���ļ�
	void OpenCameraImage(const std::string& pathJPG, std::function<void()> const& fnUpdated);

	// ��ȡ�������ͼ��
	Gdiplus::Bitmap* LockBMP();
	void UnlockBMP(Gdiplus::Bitmap* pBMP);

	// �޸ľ�ͷ����
	void UpdateCoefficients(float f, const std::vector<float>& d);
private:
	// ��ͷ����K
	cv::Mat m_k;
	// ��ͷ����D
	cv::Mat m_d;

	// ��������ͼ��
	cv::UMat m_image;
	// �������ͼ��
	cv::UMat m_imageOutG;
	cv::Mat m_imageOut;
	// ������ʾ����Ļ��
	std::unique_ptr<Gdiplus::Bitmap> m_uprtBMP;

	std::thread m_thread;
	HANDLE m_evPuls;
	std::atomic_bool m_atomQuit;
	CRITICAL_SECTION m_csKD;

	void doWork(std::function<void()> const& fnUpdated);
};


