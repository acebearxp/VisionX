#pragma once

#include "Spatial.h"

// 标准镜头光学单元
class Optica
{
public:
	Optica();
	Optica(float f135);

	// 查找theta系数表
	float FindTheta(int x, int y);
	// 计算theta
	float CalcTheta(int x, int y, int width, int height);
	
	// 生成theta系数表
	virtual void MakeTheta(int width, int height);
	// 依据光学模型,从theta计算R(像素)
	virtual float CalcRFromTheta(float fTheta, int width);

	// 计算空间变换后的坐标值
	std::tuple<int,int> SpatialTransfer(int x, int y, const SpatialDiff& delta);
protected:
	// 135全幅宽度(毫米)
	const float c_fWidthFullFrame = 36.0f;
	// 135等效焦距(毫米)
	float m_f135 = 50.0f;
	// 每一点对应的theta值,像素入射光线和光轴夹角(弧度)
	std::unique_ptr<cv::Mat> m_uptrTheta;
};

