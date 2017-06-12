#include "VisualTracking.h"
#include <fstream>
#include <sstream>
#include <iostream>  
#include <string>  
#include <io.h>
#include "OpticalFlow.h"
#include "kcftracker.h"
#include "ASMSTracker.h"
using namespace std;
#if 0
struct CvVideoCapture : cv::VideoCapture {

	double getFramesPerSecond() const {
		CvVideoCapture *const p = const_cast<CvVideoCapture *>(this);
		const double fps = p->get(cv::CAP_PROP_FPS);
		return fps ? fps : 30.0;       
	}

	int getFourCcCodec() const {
		CvVideoCapture *const p = const_cast<CvVideoCapture *>(this);
		return p->get(cv::CAP_PROP_FOURCC);
	}

	std::string getFourCcCodecString() const {
		char result[] = "????";
		CvVideoCapture *const p = const_cast<CvVideoCapture *>(this);
		const int code = p->getFourCcCodec();
		result[0] = ((code >> 0) & 0xff);
		result[1] = ((code >> 8) & 0xff);
		result[2] = ((code >> 16) & 0xff);
		result[3] = ((code >> 24) & 0xff);
		result[4] = ""[0];
		return std::string(result);
	}

	int getFrameCount() const {
		CvVideoCapture *const p = const_cast<CvVideoCapture *>(this);
		return p->get(cv::CAP_PROP_FRAME_COUNT);
	}

	cv::Size getFrameSize() const {
		CvVideoCapture *const p = const_cast<CvVideoCapture *>(this);
		const int w = p->get(cv::CAP_PROP_FRAME_WIDTH);
		const int h = p->get(cv::CAP_PROP_FRAME_HEIGHT);
		const cv::Size result(w, h);
		return result;
	}

	int getPosition(void) const {
		CvVideoCapture *const p = const_cast<CvVideoCapture *>(this);
		return p->get(cv::CAP_PROP_POS_FRAMES);
	}
	void setPosition(int p) { this->set(cv::CAP_PROP_POS_FRAMES, p); }

	CvVideoCapture(const std::string &fileName) : VideoCapture(fileName) {}
	CvVideoCapture(int n) : VideoCapture(n) {}
	CvVideoCapture() : VideoCapture() {}
};
#endif
// Called by setMouseCallback() to add new points to track.
//
static void onMouseClick(int event, int x, int y, int n, void *p)
{
	VisualTracking *const pVT = (VisualTracking *)p;
	/*
	if (event == cv::EVENT_LBUTTONDOWN) {
		pVT->roi[0] = cv::Point2f(x, y);
	}
	if (event == cv::EVENT_LBUTTONUP) {
		pVT->roi[1] = cv::Point2f(x, y);
	}
	*/
}
static double average_speed_ms = 0.0;
VisualTracking::VisualTracking(bool IsVideo, string Input,int mode)
{
	FrameNO = 0;
	average_speed_ms = 0.0;
	IsVideoFile = IsVideo;
	FileFolder = Input;
	if (Input.data()!=NULL) {
		//cv::namedWindow(Input, cv::WINDOW_AUTOSIZE);
		//cv::setMouseCallback(Input, &onMouseClick, this);
	}
	if (mode == 0)
		pTracking = new OpticalFlowTracking;
	if (mode == 1)
		pTracking = new KCFTracker;
	if (mode == 2)
		pTracking = new ASMSTracker;
}

VisualTracking::~VisualTracking()
{
	if (pTracking != NULL)
		delete pTracking;
}

bool VisualTracking::GetInput()
{
	if (IsVideoFile)
	{
	}
	else
	{
		string jpgfile = FileFolder+"\\*.jpg";
		char filename[_MAX_PATH];
		strcpy(filename, FileFolder.c_str());
		if (FrameNO==0)
		{
			if ((hFile = _findfirst(jpgfile.c_str(), &fileinfo)) != -1)
			{
				string tmp = "\\";
				strcat(filename, tmp.c_str());
				strcat(filename, fileinfo.name);
			}
			else
			{
				return false;
			}

		}
		else
		{
			if (_findnext(hFile, &fileinfo) == 0)
			{
				string tmp = "\\";
				strcat(filename, tmp.c_str());
				strcat(filename, fileinfo.name);
			}
			else
			{
				_findclose(hFile);
				return false;
			}
		}
		orig_image = cv::imread(filename);
		FrameNO++;

		// Read groundtruth for the 1st frame
		{
			ifstream groundtruthFile;
			string groundtruth = "\\groundtruth.txt";
			groundtruthFile.open(FileFolder+groundtruth);
			string firstLine;
			getline(groundtruthFile, firstLine);
			groundtruthFile.close();

			istringstream ss(firstLine);

			// Read groundtruth like a dumb
			float x1, y1, x2, y2, x3, y3, x4, y4;
			char ch;
			ss >> x1;ss >> ch;ss >> y1;ss >> ch;ss >> x2;ss >> ch;ss >> y2;ss >> ch;
			ss >> x3;ss >> ch;ss >> y3;ss >> ch;ss >> x4;ss >> ch;ss >> y4;

			// Using min and max of X and Y for groundtruth rectangle
			float xMin = min(x1, min(x2, min(x3, x4)));
			float yMin = min(y1, min(y2, min(y3, y4)));
			float width = max(x1, max(x2, max(x3, x4))) - xMin;
			float height = max(y1, max(y2, max(y3, y4))) - yMin;
			
			roi.x = xMin; roi.y = yMin; roi.width = width; roi.height = height;

		}
		
	}
	if (pTracking != NULL)
	{
		pTracking->InputImage(orig_image);
		pTracking->InputROI(roi);
	}
	
    return true;
}

void VisualTracking::RunTracking()
{
	double time_profile_counter = cv::getCPUTickCount();
	if (pTracking != NULL)
		pTracking->RunTracking();
	time_profile_counter = cv::getCPUTickCount() - time_profile_counter;
	average_speed_ms += time_profile_counter / ((double)cvGetTickFrequency() * 1000);
}

bool VisualTracking::UpdateResult(bool Showing)
{
	if (pTracking != NULL)
	{
		pTracking->showing = Showing;
		return pTracking->UpdateResult(FileFolder);
	}
	return false;
}

int main(int argc, char* argv[])
{
	int mode = -1;
	bool Showing = false;
	bool IsVideoFile = false;
	for (int i = 0; i < argc; i++){
		if (strcmp(argv[i], "of") == 0)
		{
			mode = 0;
		}	
		if (strcmp(argv[i], "kcf") == 0)
		{
			mode = 1;
		}
		if (strcmp(argv[i], "asms") == 0)
		{
			mode = 2;
		}
		if (strcmp(argv[i], "showing") == 0)
		{
			Showing = true;
		}
		if (strcmp(argv[i], "video") == 0)
		{
			IsVideoFile = true;
		}
	}
	string file = "E:\\source\\data\\vot2016\\traffic";
	VisualTracking * tracking = new VisualTracking(IsVideoFile,file,mode);
	while (tracking->GetInput())
	{
		tracking->RunTracking();
		if(!tracking->UpdateResult(Showing))
			break;
	}
	std::cout << "Average speed " << average_speed_ms / tracking->GetFrameNO() << "ms. (" << 1000.0 / (average_speed_ms / tracking->GetFrameNO()) << "fps)" << std::endl;
	delete tracking;
	
	return 0;
}