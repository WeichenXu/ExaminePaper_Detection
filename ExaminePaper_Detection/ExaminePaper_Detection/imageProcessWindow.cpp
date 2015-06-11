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
	this->oddRate = 1.5;
	this->maxRotation = 0.1;
}
imageProcessWindow::~imageProcessWindow(){
	delete imgProcess;
	if(src)	delete src;
	if(dst) delete dst;
	if(dst1) delete dst1;
}
//classify lines according to their angles
void imageProcessWindow::setLinesByAngles(){
	verLines.clear();
	horLines.clear();
	float maxSinAbs = sin(CV_PI*this->maxRotation);	// for detecting vertical and horizontal lines
	for(int i=0; i<this->lines.size();i++){
		float length = sqrtf(powf((this->lines[i][0] - this->lines[i][2]),2) + powf((this->lines[i][1] - this->lines[i][3]),2));
		float tempSin = (this->lines[i][1] - this->lines[i][3]) / length;
		float tempCos = (this->lines[i][0] - this->lines[i][2]) / length;
		if(tempSin > -maxSinAbs && tempSin < maxSinAbs){
			lineInfo temp;
			temp.index = i;
			temp.length = length;
			horLines.push_back(temp);
		}
		if(tempCos > -maxSinAbs && tempCos < maxSinAbs){
			lineInfo temp;
			temp.index = i;
			temp.length = length;
			verLines.push_back(temp);
		}
	}
}
//return the average valid line length
int imageProcessWindow::averageLine(vector<lineInfo> &lines){
	//sort and delete odd lines
	sort(lines.begin(),lines.end());
	int oddOffset = lines.size()-1,result = 0;
	for(int i=lines.size()-1; i>=0; i-- ){
		if(lines[i].length > this->oddRate*lines[0].length)	continue;
		else
		{
			oddOffset = i;
			break;
		}
	}
	lines.erase(lines.begin()+oddOffset,lines.end());
	for(int i=0; i<lines.size(); i++){
		result += lines[i].length;
	}
	return result/lines.size();
}
//get box size from lines
void imageProcessWindow::getBoxSize()
{
	this->boxHeight = averageLine(verLines);
	this->boxWidth = averageLine(horLines);
}
//filter detected lines
void imageProcessWindow::filterLines(){
	setLinesByAngles();
	getBoxSize();
	cout<<endl;
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
	filterLines();
	drawLines();
	imshow(this->windowName3,*(this->dst1));
}
void imageProcessWindow::onHoughLineTracker(int, void* param){
	imageProcessWindow *self = (imageProcessWindow *)(param);
	self->doHoughLineTracker();

}