#include "stdafx.h"
#include "Beaker.h"

using namespace std;

Beaker::Beaker()
	:c_strPath()
{
}

Beaker::Beaker(const string& strPath)
	:c_strPath(strPath)
{
	m_image = cv::imread(strPath);
}
