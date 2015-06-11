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
	this->boxes.init();
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
	// get box size
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
//set the boxboard from lines
void imageProcessWindow::setBoxBoard(){
	boxes.box_x = this->boxWidth;
	boxes.box_y = this->boxHeight;
	// add point count to box board
	for (int i=0; i<verLines.size(); i++){
		Vec4i vec = lines[verLines[i].index];
		boxes.X[vec[0]] = boxes.X[vec[0]] + 1;
		boxes.X[vec[2]] = boxes.X[vec[2]] + 1;
		boxes.Y[vec[1]] = boxes.Y[vec[1]] + 1;
		boxes.Y[vec[3]] = boxes.Y[vec[3]] + 1;
	}
	for (int i=0; i<horLines.size(); i++){
		Vec4i vec = lines[horLines[i].index];
		boxes.X[vec[0]] = boxes.X[vec[0]] + 1;
		boxes.X[vec[2]] = boxes.X[vec[2]] + 1;
		boxes.Y[vec[1]] = boxes.Y[vec[1]] + 1;
		boxes.Y[vec[3]] = boxes.Y[vec[3]] + 1;
	}
	//delete odd elements
	/*
	for(int i=1; i<MAX_IMAGE_SIZE-3; i++){
		if(!(boxes.X[i] && boxes.X[i+1] && boxes.X[i-1] || boxes.X[i] > 5))
			boxes.X[i] = 0;
		if(!(boxes.Y[i] && boxes.Y[i+1] && boxes.Y[i-1] || boxes.Y[i] > 5))
			boxes.Y[i] = 0;
	}
	*/
	
	// clear gap
	/*
	for(int i=1; i<MAX_IMAGE_SIZE-3; i++){
		if(boxes.X[i+1] && boxes.X[i-1]){
			boxes.X[i] = 1;
			boxes.X[i-1] = 0;
			boxes.X[i+1] = 0;
		}
		if(boxes.Y[i+1] && boxes.Y[i-1]){
			boxes.Y[i] = 1;
			boxes.Y[i-1] = 0;
			boxes.Y[i+1] = 0;
		}
			
	}
	*/

}

// check box board to set the valid coord
void imageProcessWindow::checkBoxBoard(){
	//merge nearby elements
	for(int i=0; i<MAX_IMAGE_SIZE; i++){
		int maxCount = 0, index, begin=i;

		while(boxes.X[i] > 0){
			maxCount = boxes.X[i];
			index = i;
			i++;
		}
		if(maxCount){
			for(int j=begin; j<index;j++)	boxes.X[j] = 0;
			for(int j=index+1; j<=i;j++)	boxes.X[j] = 0;
		}
	}
	for(int i=0; i<MAX_IMAGE_SIZE; i++){
		int maxCount = 0, index, begin=i;

		while(boxes.Y[i] > 0){
			maxCount = boxes.Y[i];
			index = i;
			i++;
		}
		if(maxCount){
			for(int j=begin; j<index;j++)	boxes.Y[j] = 0;
			for(int j=index+1; j<=i;j++)	boxes.Y[j] = 0;
		}
	}
	// delete odd
	for(int i=0; i<MAX_IMAGE_SIZE; i++){
		if(boxes.X[i] < 3)
			boxes.X[i] = 0;
		if(boxes.Y[i] < 3)
			boxes.Y[i] = 0;
	}
}
//filter detected lines
void imageProcessWindow::filterLines(){
	setLinesByAngles();
	getBoxSize();
	setBoxBoard();
	checkBoxBoard();
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
// draw detected lines
// option 1: draw all lines, 0: draw box lines
void imageProcessWindow::drawLines(int option){
	if(option == 1){
		for( size_t i = 0; i < this->lines.size(); i++ )
		{
			Vec4i l = this->lines[i];
			line( *this->dst1, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,255,255), 3, CV_AA);
		}
	}
	else if(option == 2){
		vector<int> xs, ys;
		for(int i=0; i<MAX_IMAGE_SIZE; i++){
			if(boxes.X[i])	xs.push_back(i);
			if(boxes.Y[i])	ys.push_back(i);
		}
		// draw all vers
		for(int i=0; i<ys.size()-1;i+=2){
			int start = ys[i];
			int end = ys[i+1];
			for(int j=0; j<xs.size();j++)
				line(*this->dst1, Point(xs[j],start), Point(xs[j],end), Scalar(255,255,255), 3, CV_AA);
		}
		// draw all hors
		for(int i=0; i<xs.size()-1;i+=2){
			int start = xs[i];
			int end = xs[i+1];
			for(int j=0; j<ys.size();j++)
				line(*this->dst1, Point(start, ys[j]), Point(end, ys[j]), Scalar(255,255,255), 3, CV_AA);
		}
	}
	else{
		for(int i=0; i<this->verLines.size();i++){
			Vec4i l = this->lines[verLines[i].index];
			line( *this->dst1, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,255,255), 3, CV_AA);
		}
		for(int i=0; i<this->horLines.size();i++){
			Vec4i l = this->lines[horLines[i].index];
			line( *this->dst1, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,255,255), 3, CV_AA);
		}
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
	drawLines(2);
	imshow(this->windowName3,*(this->dst1));
}
void imageProcessWindow::onHoughLineTracker(int, void* param){
	imageProcessWindow *self = (imageProcessWindow *)(param);
	self->doHoughLineTracker();

}