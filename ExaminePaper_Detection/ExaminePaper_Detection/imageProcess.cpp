// The implementation for examine paper image process
// Author: Weichen Xu
// Date: 2015/6/10
#include "imageProcess.h"
// convert to gray scale
// binarization with OTSU Threshold
// Gaussian blur
void imageProcess::preProcess(Mat& src, Mat& dst, int blurSize){
	cvtColor(src,dst,CV_RGB2GRAY);
	GaussianBlur(dst,dst,Size(blurSize,blurSize),0,0);
	threshold(dst, dst, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	return;
}

// detect edge with canny
void imageProcess::edgeDetect(Mat& src, Mat& dst, int lowThreshold, int highThreshold, int kernelSize){
	Canny(src, dst, lowThreshold, highThreshold, kernelSize);
	return;
}
