// Implementation for image process window
// Author: Weichen Xu
// Date:2015/6/10
#include "imageProcessWindow.h"
imageProcessWindow::imageProcessWindow(){
	imgProcess = new imageProcess();
	dst = new Mat;
	dst1 = new Mat;
	// parameter init
	this->gaussianSize = 3;
	this->cannyThreshold = 0;
	this->houghLineThreshold = 5;
	this->houghLineMinLin = 15;
	this->houghLineMaxGap = 10;
	this->oddRate = 0.2;
}
imageProcessWindow::~imageProcessWindow(){
	delete imgProcess;
	if(src)	delete src;
	if(dst) delete dst;
	if(dst1) delete dst1;
}

//filter detected lines
void imageProcessWindow::filterLines(){

}
// load src image
// malloc space for src
bool imageProcessWindow::loadImage(String imageName){
	Mat image = imread(imageName, 1);
	if(image.empty()){
		return false;
	}
	src = new Mat;
	*src = image;
	*dst = *src;
	imshow(this->windowName1,*(this->src));
	imshow(this->windowName2,*(this->dst));
	return true;
}
void imageProcessWindow::drawLines(){
	for( size_t i = 0; i < this->lines.size(); i++ )
	{
		Vec4i l = this->lines[i];
		line( *this->dst1, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,255,255), 3, CV_AA);
	}
}

// tracker call back function for canny detection
void imageProcessWindow::doCannyTracker(){
	imgProcess->edgeDetect(*(this->src),*(this->dst) , this->cannyThreshold, this->cannyThreshold*3, 3);
	*dst1 = *dst;
	imshow(this->windowName2,*(this->dst));
}
void imageProcessWindow::onCannyTracker(int, void* param){
	imageProcessWindow *self = (imageProcessWindow *)(param);
	self->doCannyTracker();

}

// tracker call back function for HoughLine detection
void imageProcessWindow::doHoughLineTracker(){
	*dst1 = dst->clone();
	lines.clear();
	imgProcess->detectLines(*(this->dst1),lines,this->houghLineThreshold,this->houghLineMinLin,this->houghLineMaxGap);
	cout<<"lines num: "<<lines.size()<<endl;
	drawLines();
	imshow(this->windowName3,*(this->dst1));
}
void imageProcessWindow::onHoughLineTracker(int, void* param){
	imageProcessWindow *self = (imageProcessWindow *)(param);
	self->doHoughLineTracker();

}