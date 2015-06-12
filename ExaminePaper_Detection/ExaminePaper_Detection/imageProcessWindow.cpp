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
		int minY = vec[1] < vec[3] ? vec[1]:vec[3];
		for(int j=0; j<=this->boxH.size(); j++){
			if(j == boxH.size()){
				boxLine added;
				added.start = minY;
				added.count = 1;
				boxH.push_back(added);
			}
			if(std::abs(minY - boxH[j].start) < 5){
				boxH[j].start = (boxH[j].count*boxH[j].start + minY) / (boxH[j].count + 1);
				boxH[j].count = boxH[j].count + 1;
				break;
			}
			
		}
	}
	for (int i=0; i<horLines.size(); i++){
		Vec4i vec = lines[horLines[i].index];
		boxes.X[vec[0]] = boxes.X[vec[0]] + 1;
		boxes.X[vec[2]] = boxes.X[vec[2]] + 1;
		boxes.Y[vec[1]] = boxes.Y[vec[1]] + 1;
		boxes.Y[vec[3]] = boxes.Y[vec[3]] + 1;
		int minX = vec[0] < vec[2] ? vec[0]:vec[2];
		for(int j=0; j<=this->boxW.size(); j++){
			if(j == boxW.size())
			{
				boxLine added;
				added.start = minX;
				added.count = 1;
				boxW.push_back(added);
			}
			if(std::abs(minX - boxW[j].start) < 5){
				boxW[j].start = (boxW[j].count*boxW[j].start + minX) / (boxW[j].count + 1);
				boxW[j].count = boxW[j].count + 1;
				break;
			}
			
		}
	}
}

// check box board to set the valid coord
void imageProcessWindow::checkBoxBoard(){
	for(int i=0; i<boxW.size();i++){
		// check line count
		if(boxW[i].count < 2)
			boxW.erase(boxW.begin()+i);
		// check point count
		int startX = boxW[i].start;
		int sumCount = boxes.X[startX-2] + boxes.X[startX-1] + boxes.X[startX] + boxes.X[startX+1] + boxes.X[startX+2];
		if(sumCount < 6)
			boxW.erase(boxW.begin()+i);
	}
	for(int i=0; i<boxH.size();i++){
		if(boxH[i].count < 2)
			boxH.erase(boxH.begin()+i);
		// check point count
		int startY = boxH[i].start;
		int sumCount = boxes.Y[startY-2] + boxes.Y[startY-1] + boxes.Y[startY] + boxes.Y[startY+1] + boxes.Y[startY+2];
		if(sumCount < 6)
			boxH.erase(boxH.begin()+i);
	}
	//sort and delete odd ones
	sort(boxW.begin(),boxW.end());
	sort(boxH.begin(),boxH.end());
	//merge nearby ones
	mergeNearbyLine(boxW);
	mergeNearbyLine(boxH);

	//delete odd ones
	//consider diff bitween pairs, from and only header
	for(int i=0; i<boxH.size() - 1; i++){
		float normalDiff = boxH[i+1].start - boxH[i].start;
		bool valid = true;
		for(int j=i+1; j<boxH.size() - 1; j++){
			float currentDiff = boxH[j+1].start - boxH[j].start;
			if (std::abs(currentDiff/normalDiff - 1) < 0.3)
				continue;
			valid = false;
			break;
		}
		if(!valid)	boxH.erase(boxH.begin()+i);
	}
	for(int i=0; i<boxW.size() - 1; i++){
		float normalDiff = boxW[i+1].start - boxW[i].start;
		bool valid = true;
		for(int j=i+1; j<boxW.size()-1; j++){
			float currentDiff = boxW[j+1].start - boxW[j].start;
			if (std::abs(currentDiff/normalDiff - 1) < 0.3)
				continue;
			valid = false;
			break;
		}
		if(!valid)	boxW.erase(boxW.begin()+i);
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
		// draw all vers
		for(int i=0; i<boxH.size();i++){
			int start = boxH[i].start;
			int end = start + boxHeight;
			for(int j=0; j<boxW.size();j++){
				line(*this->dst1, Point(boxW[j].start,start), Point(boxW[j].start,end), Scalar(255,255,255), 3, CV_AA);
				line(*this->dst1, Point(boxW[j].start + boxWidth,start), Point(boxW[j].start + boxWidth,end), Scalar(255,255,255), 3, CV_AA);
			}
		}
		// draw all hors
		for(int i=0; i<boxW.size();i++){
			int start = boxW[i].start;
			int end = start + boxWidth;
			for(int j=0; j<boxH.size();j++){
				line(*this->dst1, Point(start,boxH[j].start), Point(end,boxH[j].start), Scalar(255,255,255), 3, CV_AA);
				line(*this->dst1, Point(start,boxH[j].start+boxHeight), Point(end,boxH[j].start + boxHeight), Scalar(255,255,255), 3, CV_AA);
			}
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

//private functions
bool imageProcessWindow::compareBoxLoca(Vec3i &a, Vec3i &b){
	return (a[1] < b[1]);
}
void imageProcessWindow::mergeNearbyLine(vector<boxLine> & boxLines){
	for(int i=0; i<boxLines.size() - 1; i++){
		if((boxLines[i+1].start - boxLines[i].start) < 2)
			boxLines.erase(boxLines.begin()+i);
	}
}