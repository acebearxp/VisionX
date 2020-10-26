#pragma once
/* Today is October 22nd, 2020,
   release date for the Mate 40 of Huawei.
   */
#include "TeaPot.h"

using namespace std;

class Mate40
{
public:
	Mate40() {};
	virtual ~Mate40() {};

	// 图像数目
	int GetCount()const { return static_cast<int>(m_vuptrTeaPots.size()); }
	// 被处理的图像
	const vector<unique_ptr<TeaPot>>& GetTeaPots() { return m_vuptrTeaPots; }

	// 加载所有图像
	void LoadAll(const vector<string> & vPaths);

	// 鱼眼校正
	void CalibrateForFisheye();

	// 图像配准
	void SpaceMatching();
private:
	// 每张图像对应一个TeaPot
	vector<unique_ptr<TeaPot>> m_vuptrTeaPots;

	// 镜头校正参数
	float m_fFocus = 283.65f;
	vector<float> m_fD = vector<float>{ -0.05f, 0.0f, 0.0f, 0.0f };
};

