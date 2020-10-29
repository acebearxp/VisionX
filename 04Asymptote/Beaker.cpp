#include "stdafx.h"
#include "Beaker.h"

using namespace std;

Beaker::Beaker()
{

}

void Beaker::CopyImage(const Beaker& src)
{
	const cv::Mat& source = src.GetImage();

	for (int i = 0; i < m_image.rows; i++)
	{
		for (int j = 0; j < m_image.cols; j++)
		{
			uchar* pDst = m_image.data + m_image.step1() * i + 3 * j;
			uchar* pSrc = source.data + source.step1() * i + 3 * j;
			if (i < source.rows && j < source.cols)
			{
				memcpy(pDst, pSrc, 3);
			}
			else {
				// memset(pDst, 0x00, 3);
			}
		}
	}
}

void Beaker::OpticalTransfer(const Beaker& src)
{
}

void Beaker::Load(int width, int height, const cv::Vec3b& color)
{
	m_image = cv::Mat(height, width, CV_8UC3, color);
	m_strPath.clear();
}

void Beaker::Load(const std::string& path)
{
	m_image = cv::imread(path);
	m_strPath = path;
}
