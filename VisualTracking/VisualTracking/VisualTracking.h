#pragma once
#ifndef VISUALTRACKING_H
#define VISUALTRACKING_H
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video/tracking.hpp"
#include "Tracking.h"
#include <io.h>
using namespace cv;
using namespace std;

class VisualTracking
{
private:
	Mat orig_image;
	int  FrameNO;
	intptr_t hFile;
	string FileFolder;
	_finddata_t fileinfo;
	bool IsVideoFile;
	Tracking* pTracking;
	
public:
	VisualTracking(bool IsVideoFile, string input,int mode);
	~VisualTracking();
	bool GetInput();
	bool OutputVideoFile();
	void RunTracking();
	bool UpdateResult(bool);
	int GetFrameNO(){ return FrameNO; }
	Rect roi;
};

#endif