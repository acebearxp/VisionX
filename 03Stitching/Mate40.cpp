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

void Mate40::Stitching()
{
	cv::Ptr<cv::Stitcher> cvptrStitcher = cv::Stitcher::create(cv::Stitcher::Mode::SCANS);

	vector<cv::UMat> vSource;
	for (const auto& uptrTeaPot : m_vuptrTeaPots) {
		vSource.push_back(uptrTeaPot->GetImageForGPU());
	}

	cvptrStitcher->setPanoConfidenceThresh(0.1);
	cv::Stitcher::Status status = cvptrStitcher->stitch(vSource, m_xStep);
	m_uptrXStep = TeaPot::FromOpenCVImage(m_xStep);
}

void Mate40::SpaceMatching()
{
	// ����ͼ����׼
	for (int i = 1; i < m_vuptrTeaPots.size(); i++) {
		if (i + 1 <= m_vuptrTeaPots.size()) {
			// i-1 & i
			const auto& uptrLeft = m_vuptrTeaPots[i - 1];
			const auto& uptrRight = m_vuptrTeaPots[i];

			vector<cv::KeyPoint> keyPointLeft, keyPointRight;
			vector<cv::DMatch> matched;
			// C++ 17 not supported yet
			// [keyPointLeft, matched, keyPointRight] = matchAdjacent(uptrLeft->GetImageForGPU(), uptrRight->GetImageForGPU());
			tie(keyPointLeft, matched, keyPointRight) = matchAdjacent(uptrLeft->GetImageForGPU(), uptrRight->GetImageForGPU());
			cv::Mat homo3x3 = calcHomography(matched, keyPointLeft, keyPointRight);

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

tuple<vector<cv::KeyPoint>, vector<cv::DMatch>, vector<cv::KeyPoint>>
	Mate40::matchAdjacent(const cv::UMat& imageLeft, const cv::UMat& imageRight)
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
	vector<cv::DMatch> matchedPoints, matchedPointsBetter;
	matcher.match(descLeft, descRight, matchedPoints);

	// ѡ���ȽϺõ�ƥ���
	// Y��Ӧ��̫,�趨��20%����
	int maxDeltaY = static_cast<int>(imageLeft.size().height * 0.2f);
	for (const auto& matched : matchedPoints) {
		const auto& ptLeft = keyPointLeft[matched.queryIdx];
		const auto& ptRight = keyPointRight[matched.trainIdx];
		if (abs(ptLeft.pt.y - ptRight.pt.y) < maxDeltaY)
			matchedPointsBetter.push_back(matched);
	}

	wchar_t buf[1024];
	swprintf_s(buf, L"=== matched ===> %llu\n", matchedPoints.size());
	OutputDebugString(buf);

	for (const auto& point : matchedPoints) {
		const auto& ptLeft = keyPointLeft[point.queryIdx];
		const auto& ptRight = keyPointRight[point.trainIdx];
		swprintf_s(buf, L"===> %5d -> %5d : %6.2f | %5d\n", point.queryIdx, point.trainIdx, point.distance, static_cast<int>(ptLeft.pt.y-ptRight.pt.y));
		OutputDebugString(buf);
	}

	cv::drawMatches(imageLeft, keyPointLeft, imageRight, keyPointRight, matchedPointsBetter, m_xStep);
	m_uptrXStep = TeaPot::FromOpenCVImage(m_xStep);

	return { keyPointLeft, matchedPointsBetter, keyPointRight };
}

cv::Mat Mate40::calcHomography(const vector<cv::DMatch>& matched, const vector<cv::KeyPoint>& keyPointLeft, const vector<cv::KeyPoint>& keyPointRight)
{
	vector<cv::Point2f> vLeftPoints, vRightPoints;
	for (const auto& m : matched) {
		vLeftPoints.push_back(keyPointLeft[m.queryIdx].pt);
		vRightPoints.push_back(keyPointRight[m.trainIdx].pt);
	}

	// ��ͼ����,����ͼ�任����
	cv::Mat matHomo3x3 = cv::findHomography(vRightPoints, vLeftPoints, cv::RANSAC);
	stringstream ss = (stringstream() << matHomo3x3 << "\n");
	OutputDebugStringA(ss.str().c_str());

	return matHomo3x3;
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
