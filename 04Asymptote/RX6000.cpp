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
			m_vuptrBeakers.push_back(unique_ptr<Beaker>(new Beaker(path)));
		}
	}
}

void RX6000::Stitching()
{
	if (m_vuptrBeakers.size() > 0) {
		const cv::Mat& image = m_vuptrBeakers[0]->GetImage();
		m_uptrOutputBeaker = unique_ptr<Beaker>(new Beaker(image.cols, image.rows));
		m_uptrOutputBeaker->Copy(*m_vuptrBeakers[0].get());
	}
	else {
		m_uptrOutputBeaker.reset();
	}
}

void RX6000::makeTest()
{

}
