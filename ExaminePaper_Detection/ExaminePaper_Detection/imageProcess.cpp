// The implementation for examine paper image process
// Author: Weichen Xu
// Date: 2015/6/10
#include "imageProcess.h"
// convert to gray scale
// binarization with OTSU Threshold
// Gaussian blur
void imageProcess::preProcess(Mat& src, Mat& dst, int blurSize){
	if(src.type() != CV_8UC1)	
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
// detect lines in the image
// threshold = points num to be considered as a line
// minLen = min line length
// maxGap = max gap length to be merged
void imageProcess::detectLines(Mat& src, vector<Vec4i> &lines,int threshold, int minLen, int maxGap){
	HoughLinesP(src, lines, 1, CV_PI/180, threshold, minLen, maxGap );
}