#pragma once

#include "Beaker.h"

/* Today is October 29nd, 2020,
   release date for the RX6000 series of AMD.
   */
class RX6000
{
public:
	std::unique_ptr<Beaker>& GetOutputBeaker() { return m_uptrOutputBeaker; }
	const std::vector<std::unique_ptr<Beaker>>& GetBeakers() { return m_vuptrBeakers; }

	// ����ͼ��
	void LoadImages(const std::vector<std::string> vPaths);
	// �������
	void Stitching();
private:
	// ���
	std::unique_ptr<Beaker> m_uptrOutputBeaker;
	// Each beaker holds an image
	std::vector<std::unique_ptr<Beaker>> m_vuptrBeakers;

	void makeTest();
};

