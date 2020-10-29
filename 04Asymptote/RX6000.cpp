#include "stdafx.h"
#include "RX6000.h"

using namespace std;

void RX6000::LoadImages(const std::vector<std::string> vPaths)
{
	// 按顺序一一匹配上,尽可能重复利用以前创建的Beaker对象
	map<string, unique_ptr<Beaker>> mapBeakers;
	for (auto& uptrBeaker : m_vuptrBeakers) {
		mapBeakers[uptrBeaker->GetPath()] = move(uptrBeaker);
	}

	m_vuptrBeakers.clear();

	for (int i = 0; i < vPaths.size(); i++) { 
		const string& path = vPaths[i];
		auto search = mapBeakers.find(path);
		if (search != mapBeakers.end()) {
			m_vuptrBeakers.push_back(move(search->second));
		}
		else {
			auto uptrBeaker = unique_ptr<Beaker>(new Beaker());
			uptrBeaker->Load(path);
			m_vuptrBeakers.push_back(move(uptrBeaker));
		}
	}
}

void RX6000::Compute()
{
	if (m_vuptrBeakers.size() > 0) {
		const cv::Mat& image = m_vuptrBeakers[0]->GetImage();
		m_uptrOutputBeaker = unique_ptr<Beaker>(new Beaker());
		m_uptrOutputBeaker->Load(image.cols, image.rows, cv::Vec3b(0xee, 0xee, 0xee));
		m_uptrOutputBeaker->CopyImage(*m_vuptrBeakers[0].get());
	}
	else {
		m_uptrOutputBeaker.reset();
	}
}