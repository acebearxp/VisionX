#pragma once

// Each Beaker holds an image
class Beaker
{
public:
	Beaker();
	Beaker(const std::string & strPath);

	const std::string& GetPath() const { return c_strPath; }
	const cv::Mat& GetImage()const { return m_image; };
private:
	const std::string c_strPath; // ͼ��·��
	cv::Mat m_image; // ԭʼͼ��
};

