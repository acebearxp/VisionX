#pragma once

#include <Windows.h>
#include <windowsx.h>

#include <opencv2/opencv.hpp>

#ifdef _DEBUG
	#pragma comment(lib, "opencv_world440d.lib")
#else
	#pragma comment(lib, "opencv_world440.lib")
#endif //  DEBUG