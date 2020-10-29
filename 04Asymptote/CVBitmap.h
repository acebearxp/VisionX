#pragma once

// cv::Mat => Gdiplus::Bitmap
class CVBitmap
{
public:
	CVBitmap(const cv::Mat& source);
	Gdiplus::Bitmap& GetBmp()const { return *m_uptrBMP.get(); }
private:
	const cv::Mat c_mat;
	std::unique_ptr<Gdiplus::Bitmap> m_uptrBMP;
};

