#include "stdafx.h"
#include "Mate40.h"


void Mate40::clearAll()
{
	m_vuptrTeaPots.clear();

}

void Mate40::LoadAll(const vector<string>& vPaths)
{
	// ���
	m_vuptrTeaPots.clear();

	for (const string& s : vPaths) {
		m_vuptrTeaPots.push_back(unique_ptr<TeaPot>(new TeaPot(s)));
	}

	// �������
	for (const auto& uptrTeaPot : m_vuptrTeaPots) {
		uptrTeaPot->LoadImage();
	}
}

void Mate40::CalibrateForFisheye()
{
	// ����У��
	for (const auto& uptrTeaPot : m_vuptrTeaPots) {
		uptrTeaPot->Calibrate(m_fFocus, m_fD);
	}
}

void Mate40::SpaceMatching()
{
	// ����ͼ����׼
	for (int i = 1; i < m_vuptrTeaPots.size(); i++) {
		if (i + 1 <= m_vuptrTeaPots.size()) {
			// i-1 & i
			const auto& uptrLeft = m_vuptrTeaPots[i - 1];
			const auto& uptrRight = m_vuptrTeaPots[i];

			matchAdjacent(uptrLeft->GetImageForGPU(), uptrRight->GetImageForGPU());

			// ��ֻ����2��ͼ
			break;
		}

		if (i == 3 && m_vuptrTeaPots.size() == 4) {
			// TODO: ��4�ź͵�1ƴ��
			// i & 1st 
			// const auto& uptrLeft = m_vuptrTeaPots[i];
			// const auto& uptrRight = m_vuptrTeaPots[0];
			// matchAdjacent(uptrLeft->GetImageForGPU(), uptrRight->GetImageForGPU());
		}
	}
}

void Mate40::matchAdjacent(const cv::UMat& imageLeft, const cv::UMat& imageRight)
{
	// ת��Ϊ�Ҷ�
	// cv::UMat imageL, imageR;
	// cv::cvtColor(imageLeft, imageL, cv::COLOR_RGB2GRAY);
	// cv::cvtColor(imageRight, imageR, cv::COLOR_RGB2GRAY);

	// �㷨
	// cv::Ptr<cv::ORB> cvptrX = cv::ORB::create();
	// cv::Ptr<cv::BRISK> cvptrX = cv::BRISK::create(30, 5, 4.0f);
	// cv::Ptr<cv::SIFT> cvptrX = cv::SIFT::create();
	cv::Ptr<cv::AKAZE> cvptrX = cv::AKAZE::create(cv::AKAZE::DESCRIPTOR_MLDB, 0, 3, 0.002f, 4, 4, cv::KAZE::DIFF_PM_G2);

	vector<cv::KeyPoint> keyPointLeft, keyPointRight;
	cvptrX->detect(imageLeft, keyPointLeft);
	cvptrX->detect(imageRight, keyPointRight);

	// ��֪ ��ͼ���Ұ벿 �� ��ͼ����벿 ���ڶ�Ӧ��ϵ, ���Կ��Բü���һЩ��
	cv::Size sizeLeft = imageLeft.size(), sizeRight = imageRight.size();
	keyPointLeft = clipKeyPoints(keyPointLeft, 0.5f, 1.0f, sizeLeft);
	keyPointRight = clipKeyPoints(keyPointRight, 0.0f, 0.5f, sizeRight);

	cv::UMat descLeft, descRight;
	cvptrX->compute(imageLeft, keyPointLeft, descLeft);
	cvptrX->compute(imageRight, keyPointRight, descRight);

	cv::BFMatcher matcher(cv::NORM_HAMMING, true);
	// cv::FlannBasedMatcher matcher; // for SIFT
	vector<cv::DMatch> matchedPoints;
	matcher.match(descLeft, descRight, matchedPoints);

	wchar_t buf[1024];
	swprintf_s(buf, L"=== matched ===> %llu\n", matchedPoints.size());
	OutputDebugString(buf);

	
	sort(matchedPoints.begin(), matchedPoints.end(), [](const cv::DMatch& x, const cv::DMatch& y)->bool {
		return x.distance < y.distance;
	});

	// if(matchedPoints.size() > 50)
	//	matchedPoints.resize(50);

	for (const auto& point : matchedPoints) {
		const auto& ptLeft = keyPointLeft[point.queryIdx];
		const auto& ptRight = keyPointRight[point.trainIdx];
		swprintf_s(buf, L"===> %5d -> %5d : %6.2f | %5d\n", point.queryIdx, point.trainIdx, point.distance, static_cast<int>(ptLeft.pt.y-ptRight.pt.y));
		OutputDebugString(buf);
	}

	cv::drawMatches(imageLeft, keyPointLeft, imageRight, keyPointRight, matchedPoints, m_xStep);

	m_uptrXStep = TeaPot::FromOpenCVImage(m_xStep);
}

vector<cv::KeyPoint> Mate40::clipKeyPoints(const vector<cv::KeyPoint>& vKeyPoints, float fClipXFrom, float fClipXTo, const cv::Size& sizeImage)
{
	float fMin = sizeImage.width * fClipXFrom;
	float fMax = sizeImage.width * fClipXTo;

	vector<cv::KeyPoint> vKeep;
	for (const auto& point : vKeyPoints) {
		if (point.pt.x >= fMin && point.pt.x <= fMax) {
			vKeep.push_back(point);
		}
	}

	return vKeep;
}
