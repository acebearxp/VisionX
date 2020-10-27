#include "stdafx.h"
#include "Mate40.h"


void Mate40::clearAll()
{
	m_vuptrTeaPots.clear();

}

void Mate40::LoadAll(const vector<string>& vPaths)
{
	// 清空
	m_vuptrTeaPots.clear();

	for (const string& s : vPaths) {
		m_vuptrTeaPots.push_back(unique_ptr<TeaPot>(new TeaPot(s)));
	}

	// 逐个加载
	for (const auto& uptrTeaPot : m_vuptrTeaPots) {
		uptrTeaPot->LoadImage();
	}
}

void Mate40::CalibrateForFisheye()
{
	// 鱼眼校正
	for (const auto& uptrTeaPot : m_vuptrTeaPots) {
		uptrTeaPot->Calibrate(m_fFocus, m_fD);
	}
}

void Mate40::SpaceMatching()
{
	// 相邻图像配准
	for (int i = 1; i < m_vuptrTeaPots.size(); i++) {
		if (i + 1 == m_vuptrTeaPots.size()) {
			// i-1 & i
			const auto& uptrLeft = m_vuptrTeaPots[i - 1];
			const auto& uptrRight = m_vuptrTeaPots[i];

			matchAdjacent(uptrLeft->GetImageForGPU(), uptrRight->GetImageForGPU());

			// 先只处理2个图
			break;
		}

		if (i == 3 && m_vuptrTeaPots.size() == 4) {
			// TODO: 第4张和第1拼环
			// i & 1st 
			// const auto& uptrLeft = m_vuptrTeaPots[i];
			// const auto& uptrRight = m_vuptrTeaPots[0];
			// matchAdjacent(uptrLeft->GetImageForGPU(), uptrRight->GetImageForGPU());
		}
	}
}

void Mate40::matchAdjacent(const cv::UMat& imageLeft, const cv::UMat& imageRight)
{
	// 只需要匹配 "左图的右边" 和 "右图的左边"
	auto imageL = clip(imageLeft, 0.0f, 0.5f);
	auto imageR = clip(imageRight, 0.5f, 1.0f);

	// 转换为灰度
	// cv::UMat imageL, imageR;
	// cv::cvtColor(imageLL, imageL, cv::COLOR_RGB2GRAY);
	// cv::cvtColor(imageRR, imageR, cv::COLOR_RGB2GRAY);

	// 算法
	// cv::Ptr<cv::ORB> cvptrX = cv::ORB::create();
	// cv::Ptr<cv::BRISK> cvptrX = cv::BRISK::create(30, 5, 4.0f);
	// cv::Ptr<cv::SIFT> cvptrX = cv::SIFT::create();
	cv::Ptr<cv::AKAZE> cvptrX = cv::AKAZE::create(cv::AKAZE::DESCRIPTOR_MLDB, 0, 3, 0.002f, 5, 9, cv::KAZE::DIFF_PM_G2);

	vector<cv::KeyPoint> keyPointLeft, keyPointRight;
	cvptrX->detect(imageL, keyPointLeft);
	cvptrX->detect(imageR, keyPointRight);

	cv::UMat descLeft, descRight;
	cvptrX->compute(imageL, keyPointLeft, descLeft);
	cvptrX->compute(imageR, keyPointRight, descRight);

	cv::BFMatcher matcher(cv::NORM_HAMMING);
	// cv::FlannBasedMatcher matcher; // for SIFT
	vector<cv::DMatch> matchedPoints;
	matcher.match(descLeft, descRight, matchedPoints);

	wchar_t buf[1024];
	swprintf_s(buf, L"=== matched ===> %llu\n", matchedPoints.size());
	OutputDebugString(buf);

	cv::UMat xstep;
	cv::drawMatches(imageLeft, keyPointLeft, imageRight, keyPointRight, matchedPoints, xstep);
	xstep.copyTo(m_xStep);

	m_uptrXStep = TeaPot::FromOpenCVImage(m_xStep);
}

cv::UMat Mate40::clip(const cv::UMat& image, float from, float to)
{
	cv::UMat out = image.clone();
	cv::Size size = out.size();
	cv::rectangle(out, cv::Rect(static_cast<int>(size.width * from), 0, static_cast<int>(size.width * (to - from)), size.height), cv::Scalar(0,0,0xff), -1);
	return out;
}
