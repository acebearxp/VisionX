#include "stdafx.h"
#include "Mate40.h"

void Mate40::LoadAll(const vector<string>& vPaths)
{
	// 清空
	m_vuptrTeaPots.clear();

	for (const string& s : vPaths) {
		m_vuptrTeaPots.push_back(unique_ptr<TeaPot>(new TeaPot(s)));
	}

	// 逐个处理
	for (const auto& uptrTeaPot : m_vuptrTeaPots) {
		uptrTeaPot->LoadImage();
	}
}
