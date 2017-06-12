#pragma once
#ifndef OpticalFlow_H
#define OpticalFlow_H
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video/tracking.hpp"
#include "Tracking.h"
using namespace cv;
using namespace std;

class OpticalFlowTracking:public Tracking{
public:
	void InputROI(Rect groundtruth);
	void InputImage(Mat orig);
	void RunTracking();
	bool UpdateResult(std::string title);
private:
	vector<uchar> status;
	cv::Mat priorGray;              // prior frame in grayscale
	cv::Mat nextGray;               // current frame in grayscale

	std::vector<cv::Point2f> priorPoints; // tracking points in priorGray
	std::vector<cv::Point2f> nextPoints;  // tracking points in nextGray
	std::vector<cv::Mat> priorPyramid;    // flow pyramid from priorGray
	std::vector<cv::Mat> nextPyramid;     // flow pyramid from nextGray

	TermCriteria makeTerminationCriteria();
	vector<cv::Mat> makePyramid(const Mat &gray);
	vector<cv::Point2f> getGoodTrackingPoints(const Mat &gray);
	vector<uchar> calculateFlow(const vector<Mat> &priorPyramid, const vector<Point2f> &priorPoints, vector<Mat> &nextPyramid, vector<Point2f> &nextPoints);
};
#endif