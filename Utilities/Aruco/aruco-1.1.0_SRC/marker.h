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
#ifndef _Aruco_Marker_H
#define _Aruco_Marker_H
#include <vector>
#include <iostream>
#include <opencv/cv.h>
#include "exports.h"
#include "cameraparameters.h"
using namespace std;
namespace aruco {
/**\brief This class represents a marker. It is a vector of the fours corners ot the marker
 *
 */

class  ARUCO_EXPORTS Marker: public std::vector<cv::Point2f>
{
public:
    //id of  the marker
    int id;
    //size of the markers sides in meters
    float ssize;
    //matrices of rotation and translation respect to the camera
    cv::Mat Rvec,Tvec;

    /**
     */
    Marker();
    /**
     */
    Marker(const Marker &M);
    /**
     */
    Marker(const  std::vector<cv::Point2f> &corners,int _id=-1);
    /**
     */
    ~Marker() {}
    /**Indicates if this object is valid
     */
    bool isValid()const{return id!=-1 && size()==4;}

    /**Draws this marker in the input image
     */
    void draw(cv::Mat &in, cv::Scalar color, int lineWidth=1,bool writeId=true)const;

    /**Calculates the extrinsics (Rvec and Tvec) of the marker with respect to the camera
     * @param markerSize size of the marker side expressed in meters
     * @param CP parmeters of the camera
     */
    void calculateExtrinsics(float markerSize,const CameraParameters &CP);
    /**Calculates the extrinsics (Rvec and Tvec) of the marker with respect to the camera
     * @param markerSize size of the marker side expressed in meters
     * @param CameraMatrix matrix with camera parameters (fx,fy,cx,cy)
     * @param Distorsion matrix with distorsion parameters (k1,k2,p1,p2)
     */
    void calculateExtrinsics(float markerSize,cv::Mat  CameraMatrix,cv::Mat Distorsion=cv::Mat());

    /**Given the extrinsic camera parameters returns the GL_MODELVIEW matrix for opengl.
     * Setting this matrix, the reference coordinate system will be set in this marker
     */
    void glGetModelViewMatrix(  double modelview_matrix[16]);

    /**
     * Returns position vector and orientation quaternion for an Ogre scene node or entity.
     * 	Use:
     * ...
     * Ogre::Vector3 ogrePos (position[0], position[1], position[2]);
     * Ogre::Quaternion  ogreOrient (orientation[0], orientation[1], orientation[2], orientation[3]);
     * mySceneNode->setPosition( ogrePos  );
     * mySceneNode->setOrientation( ogreOrient  );
     * ...
     */
    void OgreGetPoseParameters(  double position[3], double orientation[4] );

  /**Returns the centroid of the marker
      */
    cv::Point2f getCenter()const;
     /**Returns the perimeter of the marker
      */
    float getPerimeter()const;
    /**Returns the area
     */
    float getArea()const;
    /**
     */
    /**
     */
    friend bool operator<(const Marker &M1,const Marker&M2)
    {
        return M1.id<M2.id;
    }
    /**
     */
    friend ostream & operator<<(ostream &str,const Marker &M)
    {
        str<<M.id<<"=";
        for (int i=0;i<4;i++)
            str<<"("<<M[i].x<< ","<<M[i].y<<") ";
        str<<"Txyz=";
        for (int i=0;i<3;i++)
            str<<M.Tvec.ptr<float>(0)[i]<<" ";
        str<<"Rxyz=";
        for (int i=0;i<3;i++)
            str<<M.Rvec.ptr<float>(0)[i]<<" ";

        return str;
    }

   /**
    * \brief Creates an ar marker with the id specified using a modified version of the hamming code.
    *
    * There are a total of 5 rows of 5 cols. Each row encodes a total of 2 bits, so there are 2^10 bits:(0-1023).
    *
    * The least significative bytes are first (from left-up to to right-bottom)
    *
    * Example: the id = 110 (decimal) is be represented in binary as : 00 01 10 11 10.
    *
    * Then, it will generate the following marker:
    *
    * -# 1st row encodes 00: 1 0 0 0 0 : hex 0x10
    * -# 2nd row encodes 01: 1 0 1 1 1 : hex 0x17
    * -# 3nd row encodes 10: 0 1 0 0 1 : hex 0x09
    * -# 4th row encodes 11: 0 1 1 1 0 : hex 0x0e
    * -# 5th row encodes 10: 0 1 0 0 1 : hex 0x09
    *
    * Note that : The first bit, is the inverse of the hamming parity. This avoids the 0 0 0 0 0 to be valid
    */
    static cv::Mat createMarkerImage(int id,int size);

private:
  void rotateXAxis(cv::Mat &rotation);

};

}
#endif
