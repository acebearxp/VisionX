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

	// ͼ����Ŀ
	int GetCount()const { return static_cast<int>(m_vuptrTeaPots.size()); }
	// �������ͼ��
	const vector<unique_ptr<TeaPot>>& GetTeaPots() { return m_vuptrTeaPots; }

	// ��������ͼ��
	void LoadAll(const vector<string> & vPaths);

	// ����У��
	void CalibrateForFisheye();

	// ͼ����׼
	void SpaceMatching();
private:
	// ÿ��ͼ���Ӧһ��TeaPot
	vector<unique_ptr<TeaPot>> m_vuptrTeaPots;

	// ��ͷУ������
	float m_fFocus = 283.65f;
	vector<float> m_fD = vector<float>{ -0.05f, 0.0f, 0.0f, 0.0f };
};

