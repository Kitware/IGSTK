/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*   
*     Written by: 
*      Shi Sherebrin , Robarts Research Institute - London- Ontario , www.robarts.ca
*      Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*      Claudio Gatti, Ahmad Kolahi, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2003
*
***************************************************************/
#ifndef __CAMERA_H__
#define __CAMERA_H__
#include <vector>

using namespace std;
#include "MTC.h"

class MCamera
{
public:
  MCamera(int handle = 0);
  ~MCamera();
  inline int getHandle(){ return m_handle; };
  //bool RawBufferValid();
  //unsigned char **RawBufferAddr();
  int getXRes();
  int getYRes();
  int getSerialNum();
  int getBitsPerPixel();
  double getFrameTime();
  int getNumOfFramesGrabbed();

  double getShutterTime();
  int setShutterTime(double sh);
  double getMaxShutterTime();
  double getMinShutterTime();

    double getShutterTimeLimit();
    double setShutterTimeLimit(double limit);

  double getGain();
  int setGain(double g);
  double getMinGain();
  double getMaxGain();

    double getGainLimit();
    double setGainLimit(double limit);

  mtMeasurementHazardCode getThermalHazard(); 
    
    double getDBGain();

  double getExposure();
  int setExposure(double e);
  double getMinExposure();
  double getMaxExposure();

  int getSensorsNum();

  int getAutoExposure();
  int setAutoExposure(int ae);

    double getLightCoolness();
    int setLightCoolness(double value);
  int AdjustCoolnessFromColorVector(int ColorVectorHandle);

  bool getImages( unsigned char ***li, unsigned char ***ri);
  bool getHalfSizeImages(unsigned char ***li, unsigned char ***ri, int xRes, int yRes);
  bool getImages3( unsigned char ***li, unsigned char ***ri, unsigned char ***mi);
  bool getHalfSizeImages3(unsigned char ***li, unsigned char ***ri, unsigned char ***mi, int xRes, int yRes);

  bool getProjectionOnImage( int image, double XYZ[], double *x, double *y );

  bool grabFrame(); // returns true for success
  int Handle();

private:
  int m_handle;
  bool ownedByMe;
  unsigned char *limage;
  unsigned char *rimage ;
  unsigned char *mimage ;

};

#endif
