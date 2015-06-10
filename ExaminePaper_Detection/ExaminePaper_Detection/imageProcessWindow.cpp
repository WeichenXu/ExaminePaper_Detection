// Implementation for image process window
// Author: Weichen Xu
// Date:2015/6/10
#include "imageProcessWindow.h"
imageProcessWindow::imageProcessWindow(){
	imgProcess = new imageProcess();
	dst = new Mat;
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
	return true;
}

// tracker call back function for canny detection
void imageProcessWindow::onCannyTracker(int, void*){
	imgProcess->edgeDetect(*(this->dst),*this->dst , this->cannyThreshold, this->cannyThreshold*3, 3);
}