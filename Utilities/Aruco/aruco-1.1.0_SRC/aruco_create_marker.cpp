#include <opencv2/opencv.hpp>
#include "aruco.h"
#include <iostream>
using namespace cv;
using namespace std;

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
