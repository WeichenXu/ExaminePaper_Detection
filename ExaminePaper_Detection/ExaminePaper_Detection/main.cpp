// The "Square Detector" program.
// It loads several images sequentially and tries to find squares in
// each image

#include "numBoxBoard.h"
#define MAX_BOX_LEN 20
using namespace cv;
using namespace std;
const char* dataDir = "../data";
const char* resultDir = "../result";
const char* wndname = "Square Detection";
int thresh = 50, N = 11;

static void help()
{
    cout <<
    "\nA program using Threshold Binarization, Canny, HoughLines, Box Board Detection\n"
    "memory storage (it's got it all folks) to find\n"
    "Returns sequence of chosen box on the image.\n"
    "Using OpenCV version %s\n" << CV_VERSION << "\n" << endl;
}
/*api for num box board detection*/
bool scanSpottedID(unsigned char* imgLinebyLine, int width, int height, int roiRegion[4], int inputLen, char* output, int *outputLen){
	// set up mat
	Mat testBlock = Mat(height, width, CV_8UC1, imgLinebyLine);
	Rect roi(roiRegion[0],roiRegion[1],roiRegion[2],roiRegion[3]);
	testBlock = testBlock(roi);
	imshow("imageConvert",testBlock);
	
	NumBoxBoard numBoxBoard;
	//numBoxBoard.clearData();
	numBoxBoard.preProcess();
	numBoxBoard.loadSrcFromMat(testBlock);
	numBoxBoard.doCannyTracker();
	numBoxBoard.doHoughLineTracker();
	//imshow("detected Line",*numBoxBoard.getLine());
	//imshow("detected box",*numBoxBoard.getBoxBoard());
	numBoxBoard.detectBoxNum();
	if(!numBoxBoard.setResult(inputLen, output, outputLen)) return true;
	//numBoxBoard.printResult();
	
	return 0;
	
}
int main(int /*argc*/, char** /*argv*/)
{
    static const char* names[] = 
	{	"../testData/IMG_0131.jpg"};
    help();
    
    for( int i = 0; i<1 ; i++ )
    {
		Mat image = imread(names[i], 1);
		cvtColor(image,image,CV_RGB2GRAY);
		if(image.empty()){
			continue;
		}
		unsigned char* imgByLine = image.data;
		int imgWidth = image.cols;
		int imgHeight = image.rows;
		//use entire image here for personal test
		int roiRec[4] = {0, 0, imgWidth, imgHeight};
		int inputLen = 0;
		char* output = new char[MAX_BOX_LEN];
		int* outputLen = new int(0);
		// return 0 : OK
		if(!scanSpottedID(imgByLine,imgWidth,imgHeight,roiRec,inputLen, output, outputLen)){
			cout<<"Detected result:";
			for(int i=0; i<(*outputLen); i++){
				cout<<*(output+i)<<" ";

			}
			cout<<endl;
		}
		//mainWindow.imgProcess->preProcess(*(mainWindow.src),*(mainWindow.dst),mainWindow.gaussianSize);
		//edge detection
		//create a toolbar to set a idead canny threshold
		//createTrackbar("Canny threshold", mainWindow.windowName2, &mainWindow.cannyThreshold, 500, mainWindow.onCannyTracker,&mainWindow);
		waitKey();
    }

    return 0;
}
