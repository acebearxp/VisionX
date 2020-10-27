#pragma once

using namespace std;

// ���ɵ���ͼƬ�Ĵ�������
class TeaPot
{
public:
	TeaPot(const string& path);
	virtual ~TeaPot();

	const unique_ptr<Gdiplus::Bitmap>& GetBMP() const { return m_uptrBmp; }
	const cv::UMat& GetImageForGPU()const { return m_imageGPU; }

	// ����ͼ��
	void LoadImage();
	// ���۾�ͷ����
	void Calibrate(float focus, const vector<float>& d);
public:
	// ת����BITMAP
	static unique_ptr<Gdiplus::Bitmap> FromOpenCVImage(const cv::Mat& image);

private:
	// ͼ���ļ�·��
	const string m_strPath;
	// ԭʼͼ��
	cv::Mat m_image;
	// GPUͼ��
	cv::UMat m_imageGPU;
	// ���ͼ��
	cv::Mat m_imageOut;
	// ���ͼ��BMP
	unique_ptr<Gdiplus::Bitmap> m_uptrBmp;
};

