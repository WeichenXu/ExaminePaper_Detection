// Window for image process
// Author: Weichen Xu
// Date:2015/6/10
#ifndef EXAMINE_PAPER_WINDOW
#define EXAMINE_PAPER_WINDOW

#include "imageProcess.h"

#include <iostream>
#include <math.h>
#include <string.h>

class imageProcessWindow{
public:
	imageProcessWindow();
	~imageProcessWindow();

	//source image
	Mat	*src,*dst;
	String windowName1,windowName2;
	//image process parameter
	int gaussianSize;
	int cannyThreshold;
	//load image
	bool loadImage(String imgName);
	
	imageProcess *imgProcess;

	//tracker function
	static void onCannyTracker(int ,void* param);
	void doCannyTracker();

};
#endif