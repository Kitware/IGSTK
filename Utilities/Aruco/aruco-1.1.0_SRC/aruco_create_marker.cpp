#include <opencv2/opencv.hpp>
#include "aruco.h"
#include <iostream>
using namespace cv;
using namespace std;

/** aruco_create_marker application
 *  The igstkArucoTracker can track square shaped markers using a conventional,
 *  webcam.
 *  The trackable markers can be printed on paper. In order to generate markers
 *  use this application to generate a certain marker pattern with certain
 *  resolution.
 *  Usage: <makerid(0:1023)> outfile.jpg sizeInPixels
 *  e.g. aruco_create_marker 201 marker201.jpg 200
 *
 *  The igstkArucoTracker needs to be set with marker size in mm.
 *  Make sure that printed markers have the desired size in mm.
 *  Set the desired marker size in the
 *  igstkArucoTracker using:
 *  e.g. ArucoTracker->SetMarkerSize( 50 ); // marker with 50 mm side length
 */
int main(int argc,char **argv)
{
try{
  if (argc!=4){
    cerr<<"Usage: <makerid(0:1023)> outfile.jpg sizeInPixels"<<endl;
    return -1;
  }
  Mat marker=aruco::Marker::createMarkerImage(atoi(argv[1]),atoi(argv[3]));
  imwrite(argv[2],marker);
}
catch(std::exception &ex)
{
    cout<<ex.what()<<endl;
}

}
