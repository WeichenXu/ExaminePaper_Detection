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
	//image process parameter
	int cannyThreshold;
	//load image
	bool loadImage(String imgName);
	String windowName;
	imageProcess *imgProcess;

	//tracker function
	void onCannyTracker(int, void*);

};
#endif