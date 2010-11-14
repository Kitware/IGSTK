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


#ifndef __CAMERAS_H__
#define __CAMERAS_H__
#include <vector>
#include <string>
#include "MCamera.h"

//#define MaxCameras 5

//using namespace std;

class Cameras
{
public:
  Cameras();
  ~Cameras();
  inline int getHandle() {return m_handle;};
  inline int  getCount() {return m_attachedCamNums;};
  int setHistogramEqualizeImages(bool on_off);
  bool getHistogramEqualizeImages();
  MCamera* getCamera(int index);


  int getShutterPreference();
  int setShutterPreference(int val);
  int getGainPreference();
  int setGainPreference(int val);

  int AttachAvailableCameras();
  void Detach();
  
  void SetCameraCalibrationFilesDirectory( std::string directory );
  
  inline void setMarkersHandle(int markersHandle){m_markersHandle = markersHandle;};

  bool grabFrame(MCamera *cam = NULL); // returns true for success

  // need to lear how to hide it, and keep same access syntax
  vector<MCamera *> m_vCameras;


private:
  int getMTHome( char *, int);
  int m_handle;
  bool ownedByMe;
  MCamera* mCurrCam;
  MCamera*  mFailedCam;
  std::string CalibrationDir;

  int m_attachedCamNums;
  int m_markersHandle;

};

#endif
