//Box Board detection for numbers
//Author: Weichen Xu
//Date: 2015/6/13
#ifndef NUM_BOX_BOARD
#define NUM_BOX_BOARD

#include "imageProcess.h"
#include "lineInfo.h"
#define MAX_IMAGE_SIZE 500

class NumBoxBoard{
public:
	NumBoxBoard();
	~NumBoxBoard();
	void clearData();

	//image process parameter
	int gaussianSize;
	int cannyThreshold;
	int houghLineThreshold;
	int houghLineMinLin;
	int houghLineMaxGap;
	float oddRate;
	float maxRotation;
	float boxWidth,boxHeight;

	// getter and setter
	Mat* getSrc();
	Mat* getLine();
	Mat* getBoxBoard();

	//operations
	bool loadImageFromFile(String imgName);
	bool loadSrcFromMat(Mat &src);
	void drawLines(int option);
	void setLinesByAngles();
	int averageLine(vector<lineInfo> &lines);
	void getBoxSize();
	void filterLines();
	void setBoxBoard();
	void detectBoxNum();
	void printResult();
	void preProcess();
	bool setResult(int rows, char* result, int *resultLen);
	
	//tracker function
	static void onCannyTracker(int ,void* param);
	void doCannyTracker();
	static void onHoughLineTracker(int, void* param);
	void doHoughLineTracker();

private:
	//templates for data
	int X[MAX_IMAGE_SIZE],Y[MAX_IMAGE_SIZE];
	Mat	*src,*dst,*dst1; //src = source image; dst = image after line detection; dst1 = image after box board detection
	vector<Vec4i> lines;
	vector<boxLine> boxW;
	vector<boxLine> boxH;
	vector<lineInfo> verLines;
	vector<lineInfo> horLines;
	vector<int> detectResult;
	imageProcess *imgProcess;

	//operations
	
	void checkBoxBoard();
	void mergeNearbyLine(vector<boxLine> & boxLines);
	bool compareBoxLoca(Vec3i &a, Vec3i &b);
	int boxAveragePixes(int x, int y);
};
#endif