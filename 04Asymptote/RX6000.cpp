#include "stdafx.h"
#include "RX6000.h"

using namespace std;

void RX6000::LoadImages(const std::vector<std::string> vPaths)
{
	// ��˳��һһƥ����,�������ظ�������ǰ������Beaker����
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
