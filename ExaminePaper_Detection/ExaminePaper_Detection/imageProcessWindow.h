// Window for image process
// Author: Weichen Xu
// Date:2015/6/10
#ifndef EXAMINE_PAPER_WINDOW
#define EXAMINE_PAPER_WINDOW

#include "imageProcess.h"
#include "lineInfo.h"
#include "boxBoard.h"

class imageProcessWindow{
public:
	imageProcessWindow();
	~imageProcessWindow();

	//objects
	boxBoard boxes;
	Mat	*src,*dst,*dst1;
	String windowName1,windowName2,windowName3;
	vector<Vec4i> lines;
	vector<boxLine> boxW;
	vector<boxLine> boxH;
	vector<lineInfo> verLines;
	vector<lineInfo> horLines;
	vector<int> detectResult;
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
	void drawLines(int option);
	void setLinesByAngles();
	int averageLine(vector<lineInfo> &lines);
	void getBoxSize();
	void filterLines();
	void setBoxBoard();
	void detectBoxNum();
	void printResult();
	
	//tracker function
	static void onCannyTracker(int ,void* param);
	void doCannyTracker();
	static void onHoughLineTracker(int, void* param);
	void doHoughLineTracker();

private:
	void checkBoxBoard();
	void mergeNearbyLine(vector<boxLine> & boxLines);
	bool compareBoxLoca(Vec3i &a, Vec3i &b);
	int boxAveragePixes(int x, int y);
};
#endif