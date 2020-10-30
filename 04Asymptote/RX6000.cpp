#include "stdafx.h"
#include "RX6000.h"
#include "OpticaFisheyeArea.h"
#include "OpticaFisheyeSin.h"

using namespace std;

void RX6000::LoadImages(const std::vector<std::string> vPaths)
{
	// 按顺序一一匹配上,尽可能重复利用以前创建的Beaker对象
	map<string, unique_ptr<Beaker>> mapBeakers;
	for (auto& uptrBeaker : m_vuptrBeakers) {
		const string& path = uptrBeaker->GetPath();
		if (path.empty()) continue;
		mapBeakers[path] = move(uptrBeaker);
	}

	m_vuptrBeakers.clear();

	// 方位角间隔
	const float fStepAzimuth = 2.0f * static_cast<float>(M_PI) / 4;

	for (int i = 0; i < vPaths.size(); i++) { 
		const string& path = vPaths[i];
		auto search = mapBeakers.find(path);
		if (search != mapBeakers.end()) {
			search->second->SetAzimuth(fStepAzimuth * i);
			m_vuptrBeakers.push_back(move(search->second));
		}
		else {
			auto uptrBeaker = unique_ptr<Beaker>(new Beaker());
			uptrBeaker->Load(path);
			uptrBeaker->SetAzimuth(fStepAzimuth * i);

			// auto uptrOptica = unique_ptr<Optica>(new OpticaFisheyeSin(15.0f));
			auto uptrOptica = unique_ptr<Optica>(new Optica());

			uptrBeaker->SetOptica(move(uptrOptica));
			
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
		m_uptrOutputBeaker->SetAzimuth(3.14f/4); // 北偏东
		m_uptrOutputBeaker->SetOptica(unique_ptr<Optica>(new Optica()));

		m_uptrOutputBeaker->OpticalTransfer(*m_vuptrBeakers[0].get());

		if(m_vuptrBeakers.size() > 1)
			m_uptrOutputBeaker->OpticalTransfer(*m_vuptrBeakers[1].get());
	}
	else {
		m_uptrOutputBeaker.reset();
	}
}