#pragma once
#ifndef Tracking_H
#define Tracking_H
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video/tracking.hpp"
using namespace cv;
class Tracking{
public:
	virtual void InputROI(Rect roi) = 0;
	virtual void InputImage(Mat orig) = 0;
	virtual void RunTracking() = 0;
	virtual bool UpdateResult(std::string title) = 0;
	bool showing;
protected:
	cv::Rect roi;                   
	cv::Mat image;                  // the output image in title window
};
#endif