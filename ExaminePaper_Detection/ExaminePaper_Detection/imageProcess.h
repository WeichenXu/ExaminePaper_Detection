// The header for examine paper image process
// Author: Weichen Xu
// Date: 2015/6/10
#ifndef EXAMINE_PAPER_IMAGE_PROCESS
#define EXAMINE_PAPER_IMAGE_PROCESS
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace cv;
class imageProcess{
public:
	// preprocess image
	// tracker
	void preProcess(Mat& src, Mat& dst, int blurSize);
	void preProcessTracker();

	// edge detect
	void edgeDetect(Mat& src, Mat& dst, int lowThreshold, int highThreshold, int kernelSize);
	// detect lines in the image
	void detectLines();
	// draw lines in the image
	void drawLines();
	// detect squares in the image
	void detectSquares();
	// draw squares in the image
	void drawSquares();
};
#endif