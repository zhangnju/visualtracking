#include"OpticalFlow.h"
#include <iostream>
#include <opencv2/features2d/features2d.hpp>

TermCriteria OpticalFlowTracking::makeTerminationCriteria(void)
{
	static const int criteria= TermCriteria::COUNT | TermCriteria::EPS;
	static const int iterations = 20;
	static const double epsilon = 0.03;
	return TermCriteria(criteria, iterations, epsilon);
}
// Return up to count good tracking points in gray.
//
vector<cv::Point2f> OpticalFlowTracking::getGoodTrackingPoints(const Mat &gray)
{
	static const int count = 500;
	static const double quality = 0.01;
	static const double minDistance = 10;
	static const int blockSize = 3;
	static const bool useHarrisDetector = true;
	static const double k = 0.04;
	Mat mask(gray.size(), CV_8UC1, Scalar(0));
	mask(roi).setTo(Scalar(255));
	std::vector<cv::Point2f> result;
	cv::goodFeaturesToTrack(gray, result, count, quality, minDistance,
		mask, blockSize, useHarrisDetector, k);
	static const cv::Size winSize(10, 10);
	static const cv::Size noZeroZone(-1, -1);
	static const cv::TermCriteria termCrit = makeTerminationCriteria();
	if (result.size() > 0) {
		cv::cornerSubPix(gray, result, winSize, noZeroZone, termCrit);
	}
	return result;
}

// Return an optical flow pyramid for gray.
//
std::vector<cv::Mat> OpticalFlowTracking::makePyramid(const cv::Mat &gray)
{
	static const cv::Size winSize(31, 31);
	static const int level = 3;
	std::vector<cv::Mat> result;
	cv::buildOpticalFlowPyramid(gray, result, winSize, level, true);
	return result;
}

// Calculate the flow of priorPoints in priorPyramid into nextPoints in
// nextPyramid.  For nextPoints[i], result[i] is true iff it was in
// priorPoints[i] and its flow was tracked from priorPyramid to
// nextPyramid.
//
vector<uchar> OpticalFlowTracking::calculateFlow(const vector<cv::Mat> &priorPyramid,const vector<cv::Point2f> &priorPoints,vector<Mat> &nextPyramid,vector<Point2f> &nextPoints)
{
	static const cv::Size winSize(31, 31);
	static const int level = 3;
	static const cv::TermCriteria termCrit = makeTerminationCriteria();
	static const int flags = 0;
	static const double eigenThreshold = 0.001;
	std::vector<uchar> result;
	std::vector<float> error;
	cv::calcOpticalFlowPyrLK(priorPyramid, nextPyramid,
		priorPoints, nextPoints,
		result, error, winSize, level,
		termCrit, flags, eigenThreshold);
	return result;
}

void OpticalFlowTracking::InputROI(Rect groundtruth)
{
	roi = groundtruth;
}

void OpticalFlowTracking::InputImage(Mat orig)
{
	image = orig;
	cvtColor(orig, nextGray, COLOR_BGR2GRAY);
}
void OpticalFlowTracking::RunTracking()
{
	if (priorGray.data == NULL)
	{
		nextPoints = getGoodTrackingPoints(nextGray);
		nextPyramid = makePyramid(nextGray);
	}
	else
	{
		nextPyramid = makePyramid(nextGray);
		status = calculateFlow(priorPyramid, priorPoints, nextPyramid, nextPoints);	
	}
}
static void drawGreenCircle(cv::Mat &image, const cv::Point &center)
{
	static const int radius = 3;
	static const cv::Scalar green(0, 255, 0);
	static const int thickness = cv::FILLED;
	static const int lineKind = cv::LINE_8;
	cv::circle(image, center, radius, green, thickness, lineKind);
}
bool OpticalFlowTracking::UpdateResult(std::string title)
{
	if (priorGray.data != NULL)
	{
		std::vector<cv::Point2f> result;
		const int count = nextPoints.size();
		for (int i = 0; i < count; ++i) {
			if (status[i]) {
				const cv::Point2f point = nextPoints[i];
				result.push_back(point);
			}
		}
		if (showing)
		{
			for (int i = 0; i < result.size();i++)
				drawGreenCircle(image, result[i]);
			cv::namedWindow(title, cv::WINDOW_AUTOSIZE);
			cv::imshow(title, image);
			char c = (char)cv::waitKey(0);
			if (c == 27)
				return false;
		}
		nextPoints = result;
	}
	std::swap(priorPoints, nextPoints);
	std::swap(priorGray, nextGray);
	std::swap(priorPyramid, nextPyramid);
	return true;
}