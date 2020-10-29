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

	// 加载图像
	void LoadImages(const std::vector<std::string> vPaths);
	// 处理过程
	void Stitching();
private:
	// 输出
	std::unique_ptr<Beaker> m_uptrOutputBeaker;
	// Each beaker holds an image
	std::vector<std::unique_ptr<Beaker>> m_vuptrBeakers;

	void makeTest();
};

