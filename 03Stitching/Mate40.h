#pragma once
/* Today is October 22nd, 2020,
   release date for the Mate 40 of Huawei.
   */
#include "TeaPot.h"

using namespace std;

class Mate40
{
public:
	Mate40() {};
	virtual ~Mate40() {};

	// ͼ����Ŀ
	int GetCount()const { return static_cast<int>(m_vuptrTeaPots.size()); }
	// �������ͼ��
	const vector<unique_ptr<TeaPot>>& GetTeaPots() { return m_vuptrTeaPots; }
	const unique_ptr<Gdiplus::Bitmap>& GetMergedBMP() { return m_uptrXStep; }

	// �������ͼ��
	void clearAll();
	// ��������ͼ��
	void LoadAll(const vector<string> & vPaths);

	// ����У��
	void CalibrateForFisheye();

	// ƴ��
	void Stitching();

	// ͼ����׼
	void SpaceMatching();
private:
	// ÿ��ͼ���Ӧһ��TeaPot
	vector<unique_ptr<TeaPot>> m_vuptrTeaPots;

	// ��ͷУ������
	float m_fFocus = 858.92f;
	vector<float> m_fD = vector<float>{ -1.43f, 0.39f, 3.46f, 3.44f };

	// �м���
	cv::Mat m_xStep;
	unique_ptr<Gdiplus::Bitmap> m_uptrXStep;

	// ��׼2�����ڵ�ͼ��
	tuple<vector<cv::KeyPoint>, vector<cv::DMatch>, vector<cv::KeyPoint>>
		matchAdjacent(const cv::UMat& imageLeft, const cv::UMat& imageRight);

	// ��任����
	cv::Mat calcHomography(const vector<cv::DMatch>& matched, const vector<cv::KeyPoint>& keyPointLeft, const vector<cv::KeyPoint>& keyPointRight);

	// �ü����ֹؼ���
	vector<cv::KeyPoint> clipKeyPoints(const vector<cv::KeyPoint>& vKeyPoints, float fClipXFrom, float fClipXTo, const cv::Size& sizeImage);
};

