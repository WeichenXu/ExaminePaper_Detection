// The "Square Detector" program.
// It loads several images sequentially and tries to find squares in
// each image



#include "imageProcessWindow.h"
using namespace cv;
using namespace std;
const char* dataDir = "../data";
const char* resultDir = "../result";
const char* wndname = "Square Detection";
int thresh = 50, N = 11;

static void help()
{
    cout <<
    "\nA program using pyramid scaling, Canny, contours, contour simpification and\n"
    "memory storage (it's got it all folks) to find\n"
    "squares in a list of images pic1-6.png\n"
    "Returns sequence of squares detected on the image.\n"
    "the sequence is stored in the specified memory storage\n"
    "Call:\n"
    "./squares\n"
    "Using OpenCV version %s\n" << CV_VERSION << "\n" << endl;
}
// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
static double angle( Point pt1, Point pt2, Point pt0 )
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
static void findSquares( const Mat& image, vector<vector<Point> >& squares )
{
    squares.clear();

    Mat pyr, timg, gray0(image.size(), CV_8U), gray;

    // down-scale and upscale the image to filter out the noise
    pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
    pyrUp(pyr, timg, image.size());
    vector<vector<Point> > contours;

    // find squares in every color plane of the image
    for( int c = 0; c < 3; c++ )
    {
        int ch[] = {c, 0};
        mixChannels(&timg, 1, &gray0, 1, ch, 1);

        // try several threshold levels
        for( int l = 0; l < N; l++ )
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if( l == 0 )
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                Canny(gray0, gray, 0, thresh, 5);
                // dilate canny output to remove potential
                // holes between edge segments
                dilate(gray, gray, Mat(), Point(-1,-1));
            }
            else
            {
                // apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                gray = gray0 >= (l+1)*255/N;
            }

            // find contours and store them all as a list
            findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

            vector<Point> approx;

            // test each contour
            for( size_t i = 0; i < contours.size(); i++ )
            {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                if( approx.size() == 4 &&
                    fabs(contourArea(Mat(approx))) > 100 &&
                    isContourConvex(Mat(approx)) )
                {
                    double maxCosine = 0;

                    for( int j = 2; j < 5; j++ )
                    {
                        // find the maximum cosine of the angle between joint edges
                        double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                        maxCosine = MAX(maxCosine, cosine);
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    if( maxCosine < 0.3 )
                        squares.push_back(approx);
                }
            }
        }
    }
}


// the function draws all the squares in the image
static void drawSquares( Mat& image, const vector<vector<Point> >& squares )
{
    for( size_t i = 0; i < squares.size(); i++ )
    {
        const Point* p = &squares[i][0];
        int n = (int)squares[i].size();
        polylines(image, &p, &n, 1, true, Scalar(0,255,0), 3, LINE_AA);
    }

    imshow(wndname, image);
}
//dilate the image
void Dilation(  Mat& src, Mat& dilation_dst, int dilation_elem, int dilation_size, void* )
{
  int dilation_type = 0;
  if( dilation_elem == 0 ){ dilation_type = MORPH_RECT; }
  else if( dilation_elem == 1 ){ dilation_type = MORPH_CROSS; }
  else if( dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }
  Mat element = getStructuringElement( dilation_type,
                       Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                       Point( dilation_size, dilation_size ) );
  dilate( src, dilation_dst, element );
  imshow( "Dilation Demo", dilation_dst );
}
//erode the image
void Erosion( Mat& src, Mat& erosion_dst, int erosion_elem, int erosion_size, void* )
{
  int erosion_type = 0;
  if( erosion_elem == 0 ){ erosion_type = MORPH_RECT; }
  else if( erosion_elem == 1 ){ erosion_type = MORPH_CROSS; }
  else if( erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }
  Mat element = getStructuringElement( erosion_type,
                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                       Point( erosion_size, erosion_size ) );
  erode( src, erosion_dst, element );
  imshow( "Erosion Demo", erosion_dst );
}
//proprocess image
//gaussian blur + OTSU Binarization
void preProcess( Mat& src, Mat& dst, int vis){
	int erosion_elem = 0, erosion_size = 0, max_elem=10, max_kernel_size = 10;
	Mat dst1,dst2;
	cvtColor(src,dst,CV_RGB2GRAY);
	GaussianBlur(dst,dst,Size(7,7),0,0);
	cv::threshold(dst, dst, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	// threshhold with gaussian weighted vote
	//cv::adaptiveThreshold(dst, dst2, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY,9,11);
	//string saveFileName = resultDir + string("//gaussBlur7.jpg");
	//imwrite(saveFileName,dst1);
	if(vis)	imshow(wndname,dst);
	return;
}
// define a trackbar callback
void onCannyTrackbar(int, void*)
{
    
}

int main(int /*argc*/, char** /*argv*/)
{
    static const char* names[] = { "../testData/IMG_0128.jpg", 0 };

    help();
    //namedWindow( wndname, 1 );
    vector<vector<Point> > squares;
	imageProcessWindow mainWindow;
	//parameter set		
	mainWindow.windowName1 = "src image";
	mainWindow.windowName2 = "dst image";
	mainWindow.windowName3 = "dst1 image";
	mainWindow.gaussianSize = 3;
	mainWindow.cannyThreshold = 100;
	mainWindow.houghLineThreshold = 50;
	mainWindow.houghLineMinLin = 15;
	mainWindow.houghLineMaxGap = 1;
    for( int i = 0; names[i] != 0; i++ )
    {
		mainWindow.loadImage(names[i]);
		//mainWindow.imgProcess->preProcess(*(mainWindow.src),*(mainWindow.dst),mainWindow.gaussianSize);
		
		//edge detection
		//create a toolbar to set a idead canny threshold
		createTrackbar("Canny threshold", mainWindow.windowName2, &mainWindow.cannyThreshold, 500, mainWindow.onCannyTracker,&mainWindow);
		mainWindow.doCannyTracker();
		namedWindow(mainWindow.windowName3,1);
		createTrackbar("Hough threshold", mainWindow.windowName3, &mainWindow.houghLineThreshold, 200, mainWindow.onHoughLineTracker,&mainWindow);
		createTrackbar("MinLin", mainWindow.windowName3, &mainWindow.houghLineMinLin, 50, mainWindow.onHoughLineTracker,&mainWindow);
		createTrackbar("MaxGap", mainWindow.windowName3, &mainWindow.houghLineMaxGap, 10, mainWindow.onHoughLineTracker,&mainWindow);
		mainWindow.doHoughLineTracker();
		mainWindow.detectBoxNum();
		mainWindow.printResult();

		//findSquares(result, squares);
        //drawSquares(result, squares);
        int c = waitKey();
        if( (char)c == 27 )
            break;
    }

    return 0;
}
