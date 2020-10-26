#pragma once

using namespace std;

// ���ɵ���ͼƬ�Ĵ�������
class TeaPot
{
public:
	TeaPot(const string& path);
	virtual ~TeaPot();

	const unique_ptr<Gdiplus::Bitmap>& GetBMP() const { return m_uptrBMP; }

	// ����ͼ��
	void LoadImage();

private:
	// ͼ���ļ�·��
	const string m_strPath;
	// ԭʼͼ��
	cv::Mat m_image;
	// ԭʼͼ��BMP
	unique_ptr<Gdiplus::Bitmap> m_uptrBMP;

	// ת����BITMAP
	static unique_ptr<Gdiplus::Bitmap> FromOpenCVImage(const cv::Mat& image);
};

