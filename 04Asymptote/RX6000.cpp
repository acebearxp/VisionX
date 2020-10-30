#include "stdafx.h"
#include "RX6000.h"
#include "OpticaFisheyeArea.h"
#include "OpticaFisheyeSin.h"

using namespace std;

void RX6000::LoadImages(const std::vector<std::string> vPaths)
{
	// ��˳��һһƥ����,�������ظ�������ǰ������Beaker����
	map<string, unique_ptr<Beaker>> mapBeakers;
	for (auto& uptrBeaker : m_vuptrBeakers) {
		const string& path = uptrBeaker->GetPath();
		if (path.empty()) continue;
		mapBeakers[path] = move(uptrBeaker);
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

			auto uptrOptica = unique_ptr<Optica>(new OpticaFisheyeSin(15.0f));
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
		m_uptrOutputBeaker->SetOptica(unique_ptr<Optica>(new Optica(15.0f)));

		m_uptrOutputBeaker->OpticalTransfer(*m_vuptrBeakers[0].get());
	}
	else {
		m_uptrOutputBeaker.reset();
	}
}