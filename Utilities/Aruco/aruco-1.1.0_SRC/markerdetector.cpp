/*****************************
Copyright 2011 Rafael Muñoz Salinas. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of
      conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list
      of conditions and the following disclaimer in the documentation and/or other materials
      provided with the distribution.

THIS SOFTWARE IS PROVIDED BY Rafael Muñoz Salinas ''AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Rafael Muñoz Salinas OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed
or implied, of Rafael Muñoz Salinas.
********************************/
#include "markerdetector.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <fstream>
#include "arucofidmarkers.h"

using namespace std;
using namespace cv;

namespace aruco
{
/************************************
 *
 *
 *
 *
 ************************************/
MarkerDetector::MarkerDetector()
{
    _thresMethod=ADPT_THRES;
    _thresParam1=_thresParam2=7;
    _cornerMethod=HARRIS;
    _markerWarpSize=56;
    _speed=0;
    markerIdDetector_ptrfunc=aruco::FiducidalMarkers::detect;
    pyrdown_level=0; // no image reduction
}
/************************************
 *
 *
 *
 *
 ************************************/

MarkerDetector::~MarkerDetector()
{

}

/************************************
 *
 *
 *
 *
 ************************************/
void MarkerDetector::setDesiredSpeed(int val)
{
    if (val<0) val=0;
    else if (val>3) val=2;

    _speed=val;
    switch (_speed) {

    case 0:
        _markerWarpSize=56;
        _cornerMethod=HARRIS;
        break;

    case 1:
    case 2:
        _markerWarpSize=28;
        _cornerMethod=NONE;
        break;
    };
}

/************************************
 *
 *
 *
 *
 ************************************/
void MarkerDetector::detect(const  cv::Mat &input,std::vector<Marker> &detectedMarkers, CameraParameters camParams ,float markerSizeMeters )
{
    detect( input, detectedMarkers,camParams.CameraMatrix ,camParams.Distorsion,  markerSizeMeters );
}


/************************************
 *
 * Main detection function. Performs all steps
 *
 *
 ************************************/
void MarkerDetector::detect(const  cv::Mat &input,vector<Marker> &detectedMarkers,Mat camMatrix ,Mat distCoeff ,float markerSizeMeters )
{
    //it must be a 3 channel image
    if (input.type()==CV_8UC3)
      cv::cvtColor(input,grey,CV_BGR2GRAY);
    else
      grey=input;

    //clear input data
    detectedMarkers.clear();

    cv::Mat imgToBeThresHolded=grey;
    double ThresParam1=_thresParam1,ThresParam2=_thresParam2;
    //Must the image be downsampled before continue processing?
    if (pyrdown_level!=0) {
        reduced=grey;
        for (int i=0;i<pyrdown_level;i++) {
            cv::Mat tmp;
            cv::pyrDown(reduced,tmp);
            reduced=tmp;
        }
        float red_den=pow(2.0f,pyrdown_level);
        imgToBeThresHolded=reduced;
        ThresParam1/=float(red_den);
        ThresParam2/=float(red_den);
    }

    ///Do threshold the image and detect contours
    thresHold(_thresMethod,imgToBeThresHolded,thres,ThresParam1,ThresParam2);
    //find all rectangles in the thresholdes image
    vector<std::vector<cv::Point2f> > MarkerCanditates;
    detectRectangles(thres,MarkerCanditates);
    //if the image has been downsampled, then calcualte the location of the corners in the original image
    if (pyrdown_level!=0) {
        float red_den=pow(2.0f,pyrdown_level);
        float offInc=((pyrdown_level/2.0F)-0.5F);
        for (unsigned int i=0;i<MarkerCanditates.size();i++)
            for (int c=0;c<4;c++) {
                MarkerCanditates[i][c].x=MarkerCanditates[i][c].x*red_den+offInc;
                MarkerCanditates[i][c].y=MarkerCanditates[i][c].y*red_den+offInc;
            }
    }

    ///identify the markers
    _candidates.clear();
    for (unsigned int i=0;i<MarkerCanditates.size();i++)
    {
        //Find proyective homography
        Mat canonicalMarker;
        warp(grey,canonicalMarker,Size(_markerWarpSize,_markerWarpSize),MarkerCanditates[i]);
        int nRotations;
        int id=(*markerIdDetector_ptrfunc)(canonicalMarker,nRotations);
        if (id!=-1)
        {
            detectedMarkers.push_back(MarkerCanditates[i]);
            detectedMarkers.back().id=id;
            //sort the points so that they are always in the same order no matter the camera orientation
            std::rotate(detectedMarkers.back().begin(),detectedMarkers.back().begin()+4-nRotations,detectedMarkers.back().end());
        }
        else
            _candidates.push_back(MarkerCanditates[i]);
    }
    //namedWindow("input");
    //imshow("input",input);
    //				waitKey(0);
    ///refine the corner location if desired
    if (detectedMarkers.size()>0 && _cornerMethod!=NONE)
    {
        vector<Point2f> Corners;
        for (unsigned int i=0;i<detectedMarkers.size();i++)
            for (int c=0;c<4;c++)
                Corners.push_back(detectedMarkers[i][c]);
        if (_cornerMethod==HARRIS)
            findBestCornerInRegion_harris(grey, Corners,18);
        else if (_cornerMethod==SUBPIX)
            cornerSubPix(grey, Corners,cvSize(1,1), cvSize(-1,-1)   ,cvTermCriteria ( CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,3,0.05 ));

        //copy back
        for (unsigned int i=0;i<detectedMarkers.size();i++)
            for (int c=0;c<4;c++)     detectedMarkers[i][c]=Corners[i*4+c];
    }
    //sort by id
    std::sort(detectedMarkers.begin(),detectedMarkers.end());
    //there might be still the case that a marker is detected twice because of the double border indicated earlier,
    //detect and remove these cases
    vector<bool> toRemove (detectedMarkers.size(),false);
    for (int i=0;i<int(detectedMarkers.size())-1;i++) {
        if (detectedMarkers[i].id==detectedMarkers[i+1].id && !toRemove[i+1] ) {
            //deletes the one with smaller perimeter
            if (perimeter(detectedMarkers[i])>perimeter(detectedMarkers[i+1])) toRemove[i+1]=true;
            else toRemove[i]=true;
        }
    }
    //remove the markers marker
    removeElements(detectedMarkers, toRemove);

    ///detect the position of detected markers if desired
    if (camMatrix.rows!=0  && markerSizeMeters>0)
    {
        for (unsigned int i=0;i<detectedMarkers.size();i++)
            detectedMarkers[i].calculateExtrinsics(markerSizeMeters,camMatrix,distCoeff);
    }
}


/************************************
 *
 * Crucial step. Detects the rectangular regions of the thresholded image
 *
 *
 ************************************/
void  MarkerDetector::detectRectangles(const cv::Mat &thresImg,vector<std::vector<cv::Point2f> > &MarkerCanditates)
{
    //pass a copy to findContours because the function modifies it

    std::vector<std::vector<cv::Point> > contours2;
    std::vector<cv::Vec4i> hierarchy2;

    thresImg.copyTo(thres2);
    cv::findContours( thres2 , contours2, hierarchy2,CV_RETR_TREE, CV_CHAIN_APPROX_NONE );
    vector<Point>  approxCurve;
    ///for each contour, analyze if it is a paralelepiped likely to be the marker
    for (unsigned int i=0;i<contours2.size();i++)
    {


        //check it is a possible element by first checking is has enough points
        if (contours2[i].size()>(unsigned int)(thresImg.cols /15))
        {
            //approximate to a poligon
            approxPolyDP(  Mat  (contours2[i]),approxCurve , double(contours2[i].size())*0.05 , true);
            // 				drawApproxCurve(copy,approxCurve,Scalar(0,0,255));
            //check that the poligon has 4 points
            if (approxCurve.size()==4)
            {

//  	   drawContour(input,contours2[i],Scalar(255,0,225));
//  		  namedWindow("input");
//  		imshow("input",input);
//  	 	waitKey(0);
                //and is convex
                if (isContourConvex(Mat  (approxCurve)))
                {
// 					      drawApproxCurve(input,approxCurve,Scalar(255,0,255));
// 						//ensure that the   distace between consecutive points is large enough
                    float minDist=1e10;
                    for (int k=0;k<4;k++)
                    {
                        float d= std::sqrt((float) (approxCurve[k].x-approxCurve[(k+1)%4].x)*(approxCurve[k].x-approxCurve[(k+1)%4].x) +
                                           (approxCurve[k].y-approxCurve[(k+1)%4].y)*(approxCurve[k].y-approxCurve[(k+1)%4].y));
                        // 		norm(Mat(approxCurve[k]),Mat(approxCurve[(k+1)%4]));
                        if (d<minDist) minDist=d;
                    }
                    //check that distance is not very small
                    if (minDist>10)
                    {
                        //add the points
                        // 	      cout<<"ADDED"<<endl;
                        MarkerCanditates.push_back(Marker());
                        for (int j=0;j<4;j++)
                        {
                            MarkerCanditates.back().push_back( Point2f(static_cast<float>(approxCurve[j].x),static_cast<float>(approxCurve[j].y)));
                        }
                    }
                }
            }
        }
    }
// 		 		  namedWindow("input");
//  		imshow("input",input);
//  						waitKey(0);
    ///sort the points in anti-clockwise order
    for (unsigned int i=0;i<MarkerCanditates.size();i++)
    {

        //trace a line between the first and second point.
        //if the thrid point is at the right side, then the points are anti-clockwise
        double dx1 = MarkerCanditates[i][1].x - MarkerCanditates[i][0].x;
        double dy1 =  MarkerCanditates[i][1].y - MarkerCanditates[i][0].y;
        double dx2 = MarkerCanditates[i][2].x - MarkerCanditates[i][0].x;
        double dy2 = MarkerCanditates[i][2].y - MarkerCanditates[i][0].y;
        double o = (dx1*dy2)-(dy1*dx2);

        if (o  < 0.0)		 //if the third point is in the left side, then sort in anti-clockwise order
        {
            swap(MarkerCanditates[i][1],MarkerCanditates[i][3]);

        }
    }
    /// remove these elements whise corners are too close to each other
    //first detect candidates

    vector<pair<int,int>  > TooNearCandidates;
    for (unsigned int i=0;i<MarkerCanditates.size();i++)
    {
        // 	cout<<"Marker i="<<i<<MarkerCanditates[i]<<endl;
        //calculate the average distance of each corner to the nearest corner of the other marker candidate
        for (unsigned int j=i+1;j<MarkerCanditates.size();j++)
        {
            float dist=0;
            for (int c=0;c<4;c++)
                dist+= sqrt(  (MarkerCanditates[i][c].x-MarkerCanditates[j][c].x)*(MarkerCanditates[i][c].x-MarkerCanditates[j][c].x)+(MarkerCanditates[i][c].y-MarkerCanditates[j][c].y)*(MarkerCanditates[i][c].y-MarkerCanditates[j][c].y));
            dist/=4;
            //if distance is too small
            if (dist< 10) {
                TooNearCandidates.push_back(pair<int,int>(i,j));
            }
        }
    }
    //mark for removal the element of  the pair with smaller perimeter
    vector<bool> toRemove (MarkerCanditates.size(),false);
    for (unsigned int i=0;i<TooNearCandidates.size();i++) {
        if ( perimeter(MarkerCanditates[TooNearCandidates[i].first ])>perimeter(MarkerCanditates[ TooNearCandidates[i].second] ))
            toRemove[TooNearCandidates[i].second]=true;
        else toRemove[TooNearCandidates[i].first]=true;
    }

    //remove the invalid ones
    removeElements(MarkerCanditates,toRemove);
}

/************************************
 *
 *
 *
 *
 ************************************/
void MarkerDetector::thresHold(int method,const Mat &greyImg,Mat &out,double param1,double param2)
{

    if (greyImg.type()!=CV_8UC1)     throw cv::Exception(9001,"greyImg.type()!=CV_8UC1","MarkerDetector::thresHold",__FILE__,__LINE__);
    switch (method)
    {
    case FIXED_THRES:
        cv::threshold(greyImg, out, param1,255, CV_THRESH_BINARY_INV );
        break;
    case ADPT_THRES://currently, this is the best method
//ensure that _thresParam1%2==1
        if ( param1<3) param1=3;
        else if ( ((int)param1)%2 !=1 ) param1=(int) (param1+1);

        cv::adaptiveThreshold(greyImg,out,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY_INV,static_cast<int>(param1),param2);
        break;
    case CANNY:
    {
        //this should be the best method, and generally it is.
        //However, some times there are small holes in the marker contour that makes
        //the contour detector not to find it properly
        //if there is a missing pixel
        cv::Canny(greyImg, out, 10, 220);
        //I've tried a closing but it add many more points that some
        //times makes this even worse
// 			  Mat aux;
// 			  cv::morphologyEx(thres,aux,MORPH_CLOSE,Mat());
// 			  out=aux;
    }break;
    }
}
/************************************
 *
 *
 *
 *
 ************************************/
void MarkerDetector::warp(Mat &in,Mat &out,Size size, vector<Point2f> points)
{

    if (points.size()!=4)    throw cv::Exception(9001,"point.size()!=4","MarkerDetector::warp",__FILE__,__LINE__);
    //obtain the perspective transform
    Point2f  pointsRes[4],pointsIn[4];
    for (int i=0;i<4;i++) pointsIn[i]=points[i];
    pointsRes[0]=(Point2f(0,0));
    pointsRes[1]= Point2f(size.width-1.0F,0.0F);
    pointsRes[2]= Point2f(size.width-1.0F,size.height-1.0F);
    pointsRes[3]= Point2f(0.0F,size.height-1.0F);
    Mat M=getPerspectiveTransform(pointsRes,pointsIn);
    cv::warpPerspective(in, out,  M, size,cv::INTER_NEAREST+cv::WARP_INVERSE_MAP);
}
/************************************
 *
 *
 *
 *
 ************************************/
bool MarkerDetector::isInto(Mat &contour,vector<Point2f> &b)
{

    for (unsigned int i=0;i<b.size();i++)
        if ( pointPolygonTest( contour,b[i],false)>0) return true;
    return false;
}
/************************************
 *
 *
 *
 *
 ************************************/
int MarkerDetector:: perimeter(vector<Point2f> &a)
{
    int sum=0;
    for (unsigned int i=0;i<a.size();i++) {
        int i2=(i+1)%a.size();
        sum+= static_cast<int>(sqrt ( (a[i].x-a[i2].x)*(a[i].x-a[i2].x)+(a[i].y-a[i2].y)*(a[i].y-a[i2].y) )) ;
    }
    return sum;
}


/**
 *
 *
 */
void MarkerDetector::findBestCornerInRegion_harris(const cv::Mat  & greyMat,vector<cv::Point2f> &  Corners,int blockSize)
{
    int halfSize=blockSize/2;
    for (size_t i=0;i<Corners.size();i++) {
        //check that the region is into the image limits
        cv::Point2f min(Corners[i].x-halfSize,Corners[i].y-halfSize);
        cv::Point2f max(Corners[i].x+halfSize,Corners[i].y+halfSize);
        if (min.x>=0  &&  min.y>=0 && max.x<greyMat.cols && max.y<greyMat.rows) {
            cv::Mat response;
            cv::Mat subImage(greyMat,cv::Rect(static_cast<int>(Corners[i].x)-halfSize,static_cast<int>(Corners[i].y)-halfSize,blockSize ,blockSize ));
            vector<Point2f> corners2;
            goodFeaturesToTrack(subImage, corners2, 10, 0.001, halfSize);
            double minD=9999;
            cv::Point2f Center(static_cast<float>(halfSize),static_cast<float>(halfSize));
            for (size_t j=0;j<corners2.size();j++) {
                double dist=cv::norm(corners2[j]-Center);
                if (dist<minD) {
                    minD=dist;
                }
            }
        }
    }
}


/************************************
 *
 *
 *
 *
 ************************************/
void MarkerDetector::drawAllContours(Mat input, std::vector<std::vector<cv::Point> > &contours)
{
    drawContours( input,  contours, -1,Scalar(255,0,255));
}

/************************************
 *
 *
 *
 *
 ************************************/
void MarkerDetector:: drawContour(Mat &in,vector<Point>  &contour,Scalar color  )
{
    for (unsigned int i=0;i<contour.size();i++)
    {
        cv::rectangle(in,contour[i],contour[i],color);
    }
}

void  MarkerDetector:: drawApproxCurve(Mat &in,vector<Point>  &contour,Scalar color  )
{
    for (unsigned int i=0;i<contour.size();i++)
    {
        cv::line( in,contour[i],contour[(i+1)%contour.size()],color);
    }
}
/************************************
 *
 *
 *
 *
 ************************************/

void MarkerDetector::draw(Mat out,const vector<Marker> &markers )
{
    for (unsigned int i=0;i<markers.size();i++)
    {
        cv::line( out,markers[i][0],markers[i][1],cvScalar(255,0,0),2,CV_AA);
        cv::line( out,markers[i][1],markers[i][2],cvScalar(255,0,0),2,CV_AA);
        cv::line( out,markers[i][2],markers[i][3],cvScalar(255,0,0),2,CV_AA);
        cv::line( out,markers[i][3],markers[i][0],cvScalar(255,0,0),2,CV_AA);
    }
}
/* Attempt to make it faster than in opencv. I could not :( Maybe trying with SSE3...
void MarkerDetector::warpPerspective(const cv::Mat &in,cv::Mat & out, const cv::Mat & M,cv::Size size)
{
   //inverse the matrix
   out.create(size,in.type());
   //convert to float to speed up operations
   const double *m=M.ptr<double>(0);
   float mf[9];
   mf[0]=m[0];mf[1]=m[1];mf[2]=m[2];
   mf[3]=m[3];mf[4]=m[4];mf[5]=m[5];
   mf[6]=m[6];mf[7]=m[7];mf[8]=m[8];

   for(int y=0;y<out.rows;y++){
     uchar *_ptrout=out.ptr<uchar>(y);
     for(int x=0;x<out.cols;x++){
   //get the x,y position
   float den=1./(x*mf[6]+y*mf[7]+mf[8]);
   float ox= (x*mf[0]+y*mf[1]+mf[2])*den;
   float oy= (x*mf[3]+y*mf[4]+mf[5])*den;
   _ptrout[x]=in.at<uchar>(oy,ox);
     }
   }
}
*/

/************************************
 *
 *
 *
 *
 ************************************/

void MarkerDetector::glGetProjectionMatrix( CameraParameters &  CamMatrix,cv::Size orgImgSize, cv::Size size,double proj_matrix[16],double gnear,double gfar,bool invert )
{
    cerr<<"MarkerDetector::glGetProjectionMatrix . This a deprecated function. Use CameraParameters::glGetProjectionMatrix instead. "<<__FILE__<<" "<<__LINE__<<endl;
    CamMatrix.glGetProjectionMatrix(orgImgSize,size,proj_matrix,gnear,gfar,invert);
}

};
