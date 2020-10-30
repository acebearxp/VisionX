#pragma once
#include "Optica.h"

// 包含从一个特定坐标位置用一个特定镜头拍摄的平面图像
class Beaker
{
public:
	Beaker();

	const std::string& GetPath() const { return m_strPath; }
	const cv::Mat& GetImage()const { return m_image; };
	
public:
	// 用color指定的颜色初始化图像
	void Load(int width, int height, const cv::Vec3b& color);
	// 使用指定的文件作为图像
	void Load(const std::string& path);

	// 设定光学镜头
	void SetOptica(std::unique_ptr<Optica>&& uptrOptica);

public:
	// 像素直拷
	void CopyImage(const Beaker& src);
	// 光学传送
	void OpticalTransfer(const Beaker& src);
private:
	// TODO: 坐标参数
	// 光学镜头
	std::unique_ptr<Optica> m_uptrOptica;

	std::string m_strPath; // 图像路径
	cv::Mat m_image; // 图像
};

