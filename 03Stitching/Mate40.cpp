#include "stdafx.h"
#include "Mate40.h"

void Mate40::LoadAll(const vector<string>& vPaths)
{
	// Çå¿Õ
	m_vuptrTeaPots.clear();

	for (const string& s : vPaths) {
		m_vuptrTeaPots.push_back(unique_ptr<TeaPot>(new TeaPot(s)));
	}

	// Öð¸ö¼ÓÔØ
	for (const auto& uptrTeaPot : m_vuptrTeaPots) {
		uptrTeaPot->LoadImage();
	}
}
