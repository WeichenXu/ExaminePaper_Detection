// Implementation for image process window
// Author: Weichen Xu
// Date:2015/6/10
#include "imageProcessWindow.h"
imageProcessWindow::imageProcessWindow(){
	imgProcess = new imageProcess();
	dst = new Mat;
	namedWindow( this->windowName1, 1 );
	namedWindow( this->windowName2, 1 );
	// parameter init
	this->gaussianSize = 3;
	this->cannyThreshold = 0;
}
imageProcessWindow::~imageProcessWindow(){
	delete imgProcess;
	if(src)	delete src;
	if(dst) delete dst;
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

// tracker call back function for canny detection
void imageProcessWindow::doCannyTracker(){
	imgProcess->edgeDetect(*(this->src),*(this->dst) , this->cannyThreshold, this->cannyThreshold*3, 3);
	imshow(this->windowName2,*(this->dst));
}
void imageProcessWindow::onCannyTracker(int, void* param){
	imageProcessWindow *self = (imageProcessWindow *)(param);
	self->doCannyTracker();

}