#include "stdafx.h"
#include "OpticaFisheyeTanHalf.h"

using namespace std;

OpticaFisheyeTanHalf::OpticaFisheyeTanHalf()
{
	// 默认全幅180视角, 这种鱼眼镜头可以比180度更大的视角
	constexpr float fAngleFull = static_cast<float>(M_PI);
	OpticaFisheyeTanHalf(c_fWidthFullFrame / (2.0f * tanf(fAngleFull / 4.0f)));
}

OpticaFisheyeTanHalf::OpticaFisheyeTanHalf(float f135)
	: Optica(f135)
{
}

void OpticaFisheyeTanHalf::MakeTheta(int width, int height)
{
	// 中心点(以像素为单位)
	const float fX0 = width / 2.0f;
	const float fY0 = height / 2.0f;

	// 焦距(以像素为单位)
	float fFocus = width * m_f135 / c_fWidthFullFrame;

	m_uptrTheta = make_unique<cv::Mat>(cv::Mat(height, width, CV_32F));
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			float fU = x - fX0;
			float fV = y - fY0;

			// 赤平鱼眼镜头 r=2f tan（θ/2）
			m_uptrTheta->at<float>(y, x) = 2.0f * atanf(sqrtf(fU * fU + fV * fV) / (2.0f * fFocus));
		}
	}
}

float OpticaFisheyeTanHalf::CalcRFromTheta(float fTheta, int width)
{
	// 焦距(以像素为单位)
	float fFocus = width * m_f135 / c_fWidthFullFrame;

	// 赤平鱼眼镜头 r=2f tan（θ/2） 
	return 2.0f * fFocus * tanf(fTheta / 2.0f);
}