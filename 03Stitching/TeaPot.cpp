#include "stdafx.h"
#include "TeaPot.h"

TeaPot::TeaPot(const string& path)
	:m_strPath(path)
{
	OutputDebugString(L"===> TeaPot ctor\n");
}

TeaPot::~TeaPot()
{
	OutputDebugString(L"===> TeaPot destroy\n");
}

void TeaPot::LoadImage()
{
	m_image = cv::imread(m_strPath);
}
