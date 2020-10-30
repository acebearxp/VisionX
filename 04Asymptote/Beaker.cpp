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
	cv::Vec3b blackColor = cv::Vec3b();
	// ������
	for (int y = 0; y < m_image.rows; y++) {
		for (int x = 0; x < m_image.cols; x++) {

			int x2, y2;
			tie(x2, y2) = SpatialTransfer(x, y, src);
			float fTheta = m_uptrOptica->CalcTheta(x2, y2, m_image.cols, m_image.rows);

			// ����fTheta,x,y���Ҷ�Ӧ������
			cv::Vec3b color = src.LookupPixel(fTheta, x2, y2);

			// TODO: ��ɫ��������ɫ��
			if(color != blackColor)
				m_image.at<cv::Vec3b>(y, x) = color;
		}
	}
}

std::tuple<int, int> Beaker::SpatialTransfer(int x, int y, const Beaker& src)
{	
	auto delta = m_spatial - src.m_spatial;
	return m_uptrOptica->SpatialTransfer(x, y, m_image.cols, m_image.rows, delta);
}

void Beaker::Load(int width, int height, const cv::Vec3b& color)
{
	m_image = cv::Mat(height, width, CV_8UC3, color);
	m_strPath.clear();

	// ���¼���theta
	if (m_uptrOptica)
		m_uptrOptica->MakeTheta(width, height);
}

void Beaker::Load(const std::string& path)
{
	m_image = cv::imread(path);
	m_strPath = path;

	// ���¼���theta
	if (m_uptrOptica)
		m_uptrOptica->MakeTheta(m_image.cols, m_image.rows);
}

cv::Vec3b Beaker::LookupPixel(float fTheta, int x, int y) const
{
	// ���ĵ�(������Ϊ��λ)
	const float fX0 = m_image.cols / 2.0f;
	const float fY0 = m_image.rows / 2.0f;

	float fR = m_uptrOptica->CalcRFromTheta(fTheta, m_image.cols);
	float fAngle = atan2f(y - fY0, x - fX0);
	float fU = fR * cosf(fAngle);
	float fV = fR * sinf(fAngle);

	// ����ȡ�����һ����,���õ��㷨�ǲ���ƽ��
	int nU = static_cast<int>(roundf(fX0 + fU));
	int nV = static_cast<int>(roundf(fY0 + fV));

	// �����ͼ���ⲿ����Ϊ��ɫ
	if (nU < 0 || nU >= m_image.cols || nV <0 || nV >= m_image.rows)
		return cv::Vec3b();
	else
		return m_image.at<cv::Vec3b>(nV, nU);
}

void Beaker::SetOptica(unique_ptr<Optica>&& uptrOptica)
{
	m_uptrOptica = move(uptrOptica);

	// ����theta
	if(!m_image.empty())
		m_uptrOptica->MakeTheta(m_image.cols, m_image.rows);
}
