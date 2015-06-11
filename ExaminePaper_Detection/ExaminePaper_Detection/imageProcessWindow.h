// Window for image process
// Author: Weichen Xu
// Date:2015/6/10
#ifndef EXAMINE_PAPER_WINDOW
#define EXAMINE_PAPER_WINDOW

#include "imageProcess.h"
#include "lineInfo.h"


class imageProcessWindow{
public:
	imageProcessWindow();
	~imageProcessWindow();

	//objects
	Mat	*src,*dst,*dst1;
	String windowName1,windowName2,windowName3;
	vector<Vec4i> lines;
	vector<lineInfo> verLines;
	vector<lineInfo> horLines;
	imageProcess *imgProcess;

	//image process parameter
	int gaussianSize;
	int cannyThreshold;
	int houghLineThreshold;
	int houghLineMinLin;
	int houghLineMaxGap;
	float oddRate;
	float maxRotation;
	float boxWidth,boxHeight;

	//operations
	bool loadImage(String imgName);
	void drawLines();
	void setLinesByAngles();
	int averageLine(vector<lineInfo> &lines);
	void getBoxSize();
	void filterLines();
	
	//tracker function
	static void onCannyTracker(int ,void* param);
	void doCannyTracker();
	static void onHoughLineTracker(int, void* param);
	void doHoughLineTracker();
};
#endif