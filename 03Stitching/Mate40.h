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

	// ��������ͼ��
	void LoadAll(const vector<string> & vPaths);
private:
	// ÿ��ͼ���Ӧһ��TeaPot
	vector<unique_ptr<TeaPot>> m_vuptrTeaPots;
};

