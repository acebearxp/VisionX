#include "stdafx.h"
#include "Beaker.h"

using namespace std;

Beaker::Beaker(int width, int height)
	:c_strPath()
{
	m_image = cv::Mat(height, width, CV_8UC3, cv::Scalar(0xee, 0xee, 0xee));
}

Beaker::Beaker(const string& strPath)
	:c_strPath(strPath)
{
	m_image = cv::imread(strPath);
}

void Beaker::Copy(const Beaker& src)
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
				memset(pDst, 0xee, 3);
			}
		}
	}
}

void Beaker::OpticalTransfer(const Beaker& src)
{
}
