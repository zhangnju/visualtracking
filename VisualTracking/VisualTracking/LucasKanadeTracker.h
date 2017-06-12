#ifndef _LKOF_  
#define _LKOF_  

class LucasKanadeTracker
{
	struct Point
	{
		double x;
		double y;
		Point(const double X, const double Y) :x(X), y(Y){
		}
		Point(){}
	};
	int*height;
	int*width;

private:
	unsigned int max_pyramid_layer;
	unsigned int original_imgH;
	unsigned int original_imgW;
	unsigned int window_radius;
	unsigned char**pre_pyr;//the pyramid of previous frame image,img1_pyr[0] is of max size  
	unsigned char**next_pyr;//the frame after img1_pyr  
	bool isusepyramid;
	Point*target, *endin;
	int numofpoint;


private:
	void build_pyramid(unsigned char**&original_gray);
	void lucaskanade(unsigned char**&frame_pre, unsigned char**&frame_cur, Point*& start,
		Point*& finish, unsigned int point_nums, char*state);
	void get_max_pyramid_layer();
	void pyramid_down(unsigned char*&src_gray_data, const int src_h,
		const int src_w, unsigned char*& dst, int&dst_h, int&dst_w);
	void lowpass_filter(unsigned char*&src, const int H, const int W, unsigned char*&smoothed);
	double get_subpixel(unsigned char*&src, int h, int w, const Point& point);
	// caculate the inverse mareix of pMatrix，the result is put into _pMatrix     
	void ContraryMatrix(double *pMatrix, double * _pMatrix, int dim);
	bool matrixMul(double *src1, int height1, int width1, double *src2, int height2, int width2, double *dst);
public:
	LucasKanadeTracker(const int windowRadius, bool usePyr);
	~LucasKanadeTracker();
	void get_pre_frame(unsigned char*&gray);//use only at the beginning  
	void discard_pre_frame();
	//set the next frame as pre_frame,must dicard pre_pyr in advance  
	void get_pre_frame();
	//use every time,must after using get_pre_frame(BYTE**pyr)  
	void get_next_frame(unsigned char*&gray);
	void get_info(const int nh, const int nw);
	void get_target(Point*target, int n);
	void run_single_frame();
	Point get_result();
	unsigned char*&get_pyramid(int th);
	int get_pyrH(int th){ return height[th]; }
	int get_pyrW(int th){ return width[th]; }
};

#endif  