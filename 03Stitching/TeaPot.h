#pragma once

using namespace std;

// ���ɵ���ͼƬ�Ĵ�������
class TeaPot
{
public:
	TeaPot(const string& path);
	virtual ~TeaPot();

	const unique_ptr<Gdiplus::Bitmap>& GetBMP() const { return m_uptrBmp; }

	// ����ͼ��
	void LoadImage();
	// ���۾�ͷ����
	void Calibrate(float focus, const vector<float>& d);

private:
	// ͼ���ļ�·��
	const string m_strPath;
	// ԭʼͼ��
	cv::Mat m_image;
	// ���ͼ��
	cv::Mat m_imageOut;
	// ���ͼ��BMP
	unique_ptr<Gdiplus::Bitmap> m_uptrBmp;

	// ת����BITMAP
	static unique_ptr<Gdiplus::Bitmap> FromOpenCVImage(const cv::Mat& image);
};

