#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void mCallback(int event, int x, int y, int flags, void* param){
    cv::Mat mat = *((cv::Mat*)param);
    switch(event) {
    case CV_EVENT_LBUTTONDOWN:
      std::cout << "x: " << x << std::endl;
      std::cout << "y: " << y << std::endl;
      break;
    default:
      break;
  }
}

int main( int argc, char** argv )
{
    if( argc != 2)
    {
     cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
     return -1;
    }

    Mat image;
    image = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }


    namedWindow( "test", WINDOW_AUTOSIZE );// Create a window for display.
    cv::setMouseCallback("test", mCallback, (void*) &image);
    imshow( "test", image );                   // Show our image inside it.

    waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}