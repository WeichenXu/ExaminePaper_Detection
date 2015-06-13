#include"numBoxBoard.h"
NumBoxBoard::NumBoxBoard(){
	imgProcess = new imageProcess();
	src = new Mat();
	dst = new Mat();
	dst1 = new Mat();
	// parameter init
	this->gaussianSize = 3;
	this->cannyThreshold = 100;
	this->houghLineThreshold = 50;
	this->houghLineMinLin = 15;
	this->houghLineMaxGap = 1;
	this->oddRate = 1.5;
	this->maxRotation = 0.1;
	int i = 0;
	while(i<MAX_IMAGE_SIZE){
		X[i] = 0;
		Y[i] = 0;
		i++;
	}
}
NumBoxBoard::~NumBoxBoard(){
	delete imgProcess;
	if(src)	delete src;
	if(dst)	delete dst;
	if(dst1)	delete dst1;
}
//classify lines according to their angles
void NumBoxBoard::setLinesByAngles(){
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
int NumBoxBoard::averageLine(vector<lineInfo> &lines){
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
void NumBoxBoard::getBoxSize()
{
	this->boxHeight = averageLine(verLines);
	this->boxWidth = averageLine(horLines);
}
//set the boxboard from lines
void NumBoxBoard::setBoxBoard(){
	// add point count to box board
	for (int i=0; i<verLines.size(); i++){
		Vec4i vec = lines[verLines[i].index];
		X[vec[0]] = X[vec[0]] + 1;
		X[vec[2]] = X[vec[2]] + 1;
		Y[vec[1]] = Y[vec[1]] + 1;
		Y[vec[3]] = Y[vec[3]] + 1;
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
		X[vec[0]] = X[vec[0]] + 1;
		X[vec[2]] = X[vec[2]] + 1;
		Y[vec[1]] = Y[vec[1]] + 1;
		Y[vec[3]] = Y[vec[3]] + 1;
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
void NumBoxBoard::checkBoxBoard(){
	for(int i=0; i<boxW.size();i++){
		// check line count
		if(boxW[i].count < 2)
			boxW.erase(boxW.begin()+i);
		// check point count
		int startX = boxW[i].start;
		int sumCount = X[startX-2] + X[startX-1] + X[startX] + X[startX+1] + X[startX+2];
		if(sumCount < 6)
			boxW.erase(boxW.begin()+i);
	}
	for(int i=0; i<boxH.size();i++){
		if(boxH[i].count < 2)
			boxH.erase(boxH.begin()+i);
		// check point count
		int startY = boxH[i].start;
		int sumCount = Y[startY-2] + Y[startY-1] + Y[startY] + Y[startY+1] + Y[startY+2];
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
void NumBoxBoard::filterLines(){
	setLinesByAngles();
	getBoxSize();
	setBoxBoard();
	checkBoxBoard();
}
// load src image
// malloc space for src
bool NumBoxBoard::loadImageFromFile(String imageName){
	Mat image = imread(imageName, 1);
	if(image.empty()){
		return false;
	}
	*src = image.clone();
	*dst = src->clone();
	*dst1= src->clone();
	return true;
}
// load src mat
bool NumBoxBoard::loadSrcFromMat(Mat &m){
	*src = m.clone();
	*dst = src->clone();
	*dst1= src->clone();
	return true;
}
//preoprocess
void NumBoxBoard::preProcess(){
	imgProcess->preProcess(*src,*dst,gaussianSize);
}
// draw detected lines
// option 1: draw all lines, 0: draw box lines
void NumBoxBoard::drawLines(int option){
	if(option == 1){
		for( size_t i = 0; i < this->lines.size(); i++ )
		{
			Vec4i l = this->lines[i];
			line( *this->dst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,255,255), 3, CV_AA);
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
			line( *this->dst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,255,255), 3, CV_AA);
		}
		for(int i=0; i<this->horLines.size();i++){
			Vec4i l = this->lines[horLines[i].index];
			line( *this->dst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,255,255), 3, CV_AA);
		}
	}
}
// tracker call back function for canny detection
void NumBoxBoard::doCannyTracker(){
	imgProcess->edgeDetect(*(this->src),*(this->dst) , this->cannyThreshold, this->cannyThreshold*3, 3);
}
void NumBoxBoard::onCannyTracker(int, void* param){
	NumBoxBoard *self = (NumBoxBoard *)(param);
	self->doCannyTracker();
}
// tracker call back function for HoughLine detection
void NumBoxBoard::doHoughLineTracker(){
	*dst1 = dst->clone();
	lines.clear();
	imgProcess->detectLines(*(this->dst1),lines,this->houghLineThreshold,this->houghLineMinLin,this->houghLineMaxGap);
	cout<<"lines num: "<<lines.size()<<endl;
	filterLines();
	drawLines(1);
	drawLines(2);
}
void NumBoxBoard::onHoughLineTracker(int, void* param){
	NumBoxBoard *self = (NumBoxBoard *)(param);
	self->doHoughLineTracker();
}
// detect chosen num in each col of boxes
void NumBoxBoard::detectBoxNum(){
	for(int i=0; i<boxW.size(); i++){
		int minIntensity = 255, avgIntensity = 0;
		int minIntenNum;
		for(int j=0; j<boxH.size(); j++){
			int blockIntensity = boxAveragePixes(boxW[i].start,boxH[j].start);
			avgIntensity += blockIntensity;
			//cout<<blockIntensity<<" "<<endl;
			if(blockIntensity < minIntensity){
				minIntenNum = j;
				minIntensity = blockIntensity;
			}
		}
			//cout<<endl;
		avgIntensity = avgIntensity / boxH.size();
		if(minIntensity < 0.9*avgIntensity)
			detectResult.push_back(minIntenNum);
		else
			detectResult.push_back(-1);
		
	}

}
//print detected num
void NumBoxBoard::printResult(){
	for(int i=0; i<detectResult.size(); i++)
		cout<<detectResult[i]<<" ";
	cout<<endl;
}
//set detected result to the char array
bool NumBoxBoard::setResult(int rows,char* result, int *resultLen){
	if(rows != 0 && rows!= detectResult.size())
		return false;
	*resultLen = detectResult.size();
	for(int i=0; i<detectResult.size(); i++){
		if(detectResult[i] == -1)	*(result+i) = 'u';
		else
		{
			*(result + i) = '0' + detectResult[i];
		}
	}
	return true;
}
//getter & setter
Mat* NumBoxBoard::getSrc(){
	return src;
}
Mat* NumBoxBoard::getLine(){
	return dst;
}
Mat* NumBoxBoard::getBoxBoard(){
	return dst1;
}

//private functions
bool NumBoxBoard::compareBoxLoca(Vec3i &a, Vec3i &b){
	return (a[1] < b[1]);
}
void NumBoxBoard::mergeNearbyLine(vector<boxLine> & boxLines){
	for(int i=0; i<boxLines.size() - 1; i++){
		if((boxLines[i+1].start - boxLines[i].start) < 2)
			boxLines.erase(boxLines.begin()+i);
	}
}
int NumBoxBoard::boxAveragePixes(int x, int y){
	Rect roi(x,y,boxWidth,boxHeight);
	Mat block = (*src)(roi);
	
	Scalar result = cv::mean(block);
	return result.val[0];
}
void NumBoxBoard::clearData(){
	int i = 0;
	while(i<MAX_IMAGE_SIZE){
		X[i] = 0;
		Y[i] = 0;
		i++;
	}
	lines.clear();
	boxW.clear();
	boxH.clear();
	verLines.clear();
	horLines.clear();
	detectResult.clear();
}